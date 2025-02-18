#include "ResUtil.hpp"
#include "ini.h"
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <format>
#include <ImGuiFileDialog.h>

SResUtility::SOptions Options;

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
		ImGui::Text(std::format("Objects Path: {0}", mRootDir == "" ? "(Not Set)" : mRootDir.string()).data());
		ImGui::SameLine();
		if(ImGui::Button("Open")){
			mSelectRootDialogOpen = true;
		}

		if(ImGui::Button("Save")){
			std::ofstream settingsFile(std::filesystem::current_path() / "settings.ini");
			settingsFile << std::format("[settings]\nobject_dir={0}", mRootDir.string());
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