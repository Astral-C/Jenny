#pragma once

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <filesystem>

#include <J3D/J3DModelLoader.hpp>
#include <J3D/J3DModelData.hpp>
#include <J3D/J3DUniformBufferObject.hpp>
#include <J3D/J3DLight.hpp>
#include <J3D/J3DModelInstance.hpp>
#include "io/GenParser.hpp"
#include "ResUtil.hpp"
#include "UPathRenderer.hpp"

#include "archive.h"

#include "UCamera.hpp"

class PMapManager {
	bool loadedAssets { false };
	
	void LoadEnemy(std::string modelDirName);
	void LoadTreasure(std::string modelArchiveName);
	void LoadKandoModel(std::string modelDirName, std::string modelName, std::string cacheName, std::string archiveName);

	void ParseGens(std::filesystem::path genPath);

	void RenderGen(std::shared_ptr<PGenerator> gen, float dt);

	CPathRenderer pathRenderer;

public:
	PGenCollection defaultGens, initGens, plantsGen;
	// I think these refer to days? day specific??
	
	std::map<std::string, PGenCollection> loops;
	std::map<std::string, PGenCollection> nonloops;

	std::map<std::string, bool> loopVisibility;
	std::map<std::string, bool> nonLoopVisibility;

	PRoute routes;

	void RenderUI();
	void RenderMap(float dt, USceneCamera* camera);
	void LoadMap(std::string mapName);
	void LoadAssets();

	PMapManager();
	~PMapManager();
};