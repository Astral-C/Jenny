#pragma once

#include "GenUtil.hpp"
#include "archive.h"
#include "compression.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <ini.h>

namespace SResUtility
{
	class SGCResourceManager
	{
		bool mInitialized { false };
		GCcontext mResManagerContext;
		public:
			bool LoadArchive(const char* path, GCarchive* archive);
			bool SaveArchiveCompressed(const char* path, GCarchive* archive);
			bool ReplaceArchiveFileData(GCarcfile* file, uint8_t* new_data, size_t new_data_size);
			GCarcfile* GetFile(GCarchive* archive, std::filesystem::path filepath);
			GCarcfile* GetFirstFileWithExtension(GCarchive* archive, std::string extension);
			void Init();
	};

	class SOptions //any sort of options will be here
	{
		bool mSelectRootDialogOpen;
		
		public:
			std::filesystem::path mRootDir;

			void RenderOptionMenu();
			void LoadOptions();
	};
}

extern SResUtility::SGCResourceManager GCResourceManager;
extern SResUtility::SOptions Options;