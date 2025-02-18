#pragma once

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include <J3D/J3DModelLoader.hpp>
#include <J3D/Data/J3DModelData.hpp>
#include <J3D/Material/J3DUniformBufferObject.hpp>
#include <J3D/Rendering/J3DLight.hpp>
#include <J3D/Data/J3DModelInstance.hpp>
#include <J3D/Picking/J3DPicking.hpp>
#include <J3D/Rendering/J3DRendering.hpp>

#include "io/GenParser.hpp"
#include "ResUtil.hpp"
#include "UPathRenderer.hpp"

#include <Archive.hpp>

#include "UCamera.hpp"

class PMapManager {
	bool loadedAssets { false };
	
	void LoadEnemy(std::string modelDirName);
	void LoadTreasure(std::string modelArchiveName);
	void LoadKandoModel(std::string modelDirName, std::string modelName, std::string cacheName, std::string archiveName);

	void ParseGens(std::filesystem::path genPath);

	void RenderGen(std::shared_ptr<PGenerator> gen, float dt);

	CPathRenderer pathRenderer;
	std::shared_ptr<J3DModelInstance> mMapModel { nullptr };

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