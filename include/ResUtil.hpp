#pragma once

#include "GenUtil.hpp"
#include <Archive.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <map>
#include <ini.h>

namespace SResUtility
{

	class SOptions //any sort of options will be here
	{
		bool mSelectRootDialogOpen;
		
		public:
			bool mRootChanged { false };
			std::filesystem::path mRootDir;

			void RenderOptionMenu();
			void LoadOptions();
	};
}

extern SResUtility::SOptions Options;