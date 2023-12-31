#include "ResUtil.hpp"
#include "ini.h"
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <fmt/core.h>
#include <ImGuiFileDialog.h>

SResUtility::SGCResourceManager GCResourceManager;
SResUtility::SOptions Options;

void SResUtility::SGCResourceManager::Init()
{
	GCerror err;
	if ((err = gcInitContext(&mResManagerContext)) != GC_ERROR_SUCCESS)
	{
		printf("Error initing arc loader context: %s\n", gcGetErrorMessage(err));
	}

	mInitialized = true;
}

GCarcfile* SResUtility::SGCResourceManager::GetFile(GCarchive* archive, std::filesystem::path filepath){
	int dirID = 0;
	for(std::filesystem::path component : filepath){
		for (GCarcfile* file = &archive->files[archive->dirs[dirID].fileoff]; file < &archive->files[archive->dirs[dirID].fileoff] + archive->dirs[dirID].filenum; file++){
			if(strcmp(file->name, component.string().c_str()) == 0 && (file->attr & 0x02)){
				dirID = file->size;
				break;
			} else if(strcmp(file->name, component.string().c_str()) == 0 && !(file->attr & 0x02)) {
				return file;
			}
		}
	}
	return nullptr;
}

GCarcfile* SResUtility::SGCResourceManager::GetFirstFileWithExtension(GCarchive* archive, std::string extension){
	int dirID = 0;
	for (GCarcfile* file = archive->files; file < archive->files + archive->filenum; file++){
		if(std::filesystem::path(file->name).extension() == extension && !(file->attr & 0x02)) {
			return file;
		}
	}
	return nullptr;
}

bool SResUtility::SGCResourceManager::LoadArchive(const char* path, GCarchive* archive)
{
	if(!mInitialized) return false;
	
	GCerror err;

	FILE* f = fopen(path, "rb");
	if (f == nullptr)
	{
		printf("Error opening file \"%s\"\n", path);
		return false;
	}

	fseek(f, 0L, SEEK_END);
	GCsize size = (GCsize)ftell(f);
	rewind(f);

	void* file = malloc(size);
	if (file == nullptr)
	{
		printf("Error allocating buffer for file \"%s\"\n", path);
		return false;
	}

	fread(file, 1, size, f);
	fclose(f);

	// If the file starts with 'Yay0', it's Yay0 compressed.
	if (*((uint32_t*)file) == 0x30796159)
	{
		GCsize compressedSize = gcDecompressedSize(&mResManagerContext, (GCuint8*)file, 0);

		void* decompBuffer = malloc(compressedSize);
		gcYay0Decompress(&mResManagerContext, (GCuint8*)file, (GCuint8*)decompBuffer, compressedSize, 0);

		free(file);
		size = compressedSize;
		file = decompBuffer;
	}
	// Likewise, if the file starts with 'Yaz0' it's Yaz0 compressed.
	else if (*((uint32_t*)file) == 0x307A6159)
	{
		GCsize compressedSize = gcDecompressedSize(&mResManagerContext, (GCuint8*)file, 0);

		void* decompBuffer = malloc(compressedSize);
		gcYaz0Decompress(&mResManagerContext, (GCuint8*)file, (GCuint8*)decompBuffer, compressedSize, 0);
		free(file);
		size = compressedSize;
		file = decompBuffer;
	}

	gcInitArchive(archive, &mResManagerContext);
	if ((err = gcLoadArchive(archive, file, size)) != GC_ERROR_SUCCESS) {
		printf("Error Loading Archive: %s\n", gcGetErrorMessage(err));
		return false;
	}

	return true;
}

bool SResUtility::SGCResourceManager::ReplaceArchiveFileData(GCarcfile* file, uint8_t* new_data, size_t new_data_size){
	if(!mInitialized) return false;
	
	// free existing file
	gcFreeMem(&mResManagerContext, file->data);

	//allocate size of new file
	file->data = gcAllocMem(&mResManagerContext, new_data_size);
		
	//copy new jmp to file buffer for arc
	memcpy(file->data, new_data, new_data_size);

	//set size properly
	file->size = new_data_size;

	return true;
}

bool SResUtility::SGCResourceManager::SaveArchiveCompressed(const char* path, GCarchive* archive)
{
	if(!mInitialized) return false;

	GCsize outSize = gcSaveArchive(archive, NULL);
	GCuint8* archiveOut = new GCuint8[outSize];
	GCuint8* archiveCmp = new GCuint8[outSize];

	gcSaveArchive(archive, archiveOut);
	// TODO: Add option to save compressed for if we want to save a yaz0 compressed file
	GCsize cmpSize = gcYay0Compress(&mResManagerContext, archiveOut, archiveCmp, outSize);
	
	std::ofstream fileStream;
	fileStream.open(path, std::ios::binary | std::ios::out);
	fileStream.write((const char*)archiveCmp, cmpSize);
	fileStream.close();

	delete archiveOut;
	delete archiveCmp;

	return true;
}

void SResUtility::SOptions::LoadOptions(){
	auto optionsPath = std::filesystem::current_path() / "settings.ini";
	if(std::filesystem::exists(optionsPath)){
		ini_t* config = ini_load(optionsPath.string().c_str());
		if(config == nullptr) return;

		const char* path = ini_get(config, "settings", "object_dir");
		if(path != nullptr) mRootDir = std::filesystem::path(path);
		ini_free(config);
	}
}

void SResUtility::SOptions::RenderOptionMenu(){
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Options", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text(fmt::format("Objects Path: {0}", mRootDir == "" ? "(Not Set)" : mRootDir.string()).data());
		ImGui::SameLine();
		if(ImGui::Button("Open")){
			mSelectRootDialogOpen = true;
		}

		if(ImGui::Button("Save")){
			std::ofstream settingsFile(std::filesystem::current_path() / "settings.ini");
			settingsFile << fmt::format("[settings]\nobject_dir={0}", mRootDir.string());
			settingsFile.close();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if(ImGui::Button("Close")){
			ImGui::CloseCurrentPopup();
		}

		if(mSelectRootDialogOpen && !ImGuiFileDialog::Instance()->IsOpened("OpenRootDialog")){
			ImGuiFileDialog::Instance()->Close();
			ImGuiFileDialog::Instance()->OpenDialog("OpenRootDialog", "Choose ObjectData path", nullptr, ".");
		}

		if (ImGuiFileDialog::Instance()->Display("OpenRootDialog")) {
			if (ImGuiFileDialog::Instance()->IsOk()) {
				mRootDir = ImGuiFileDialog::Instance()->GetFilePathName();

				mSelectRootDialogOpen = false;
			} else {
				mSelectRootDialogOpen = false;
			}

			ImGuiFileDialog::Instance()->Close();
		}
		ImGui::EndPopup();
	}
}