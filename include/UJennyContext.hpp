#pragma once

#include "UCamera.hpp"

#include <vector>
#include <filesystem>
#include <memory>
#include <UGrid.hpp>
#include <UPointSpriteManager.hpp>
#include <UMap.hpp>
#include <GCM.hpp>

#include <ImGuiFileDialog.h>

namespace bStream { class CStream; }
class J3DModelData;
class J3DMaterial;
class J3DModelInstance;


class UJennyContext {
	
	// mRoot;
	USceneCamera mCamera;
	UGrid mGrid;

	UPointSpriteManager mBillboardManager;
	PMapManager mMapManager;
	std::shared_ptr<PGenerator> selected { nullptr };

	std::shared_ptr<Disk::Image> mImage { nullptr };

	std::string mMapSelected { "tutorial" };

	uint32_t mMainDockSpaceID;
	uint32_t mDockNodeBottomID, mDockNodeRightID, mDockNodeBottomRightID;
	
	bool bIsDockingSetUp { false };
	bool bIsMapSelectDialogOpen { false };

	bool mSetLights { false };

	bool bOptionsOpen { false };

	uint32_t mFbo, mRbo, mViewTex, mPickTex;

	float mPrevWinWidth { -1.0f };
	float mPrevWinHeight { -1.0f };

	void RenderMainWindow(float deltaTime);
	void RenderPanels(float deltaTime);
	void RenderMenuBar();

	void OpenModelCB();
	void SaveModelCB();

	void SetLights();
	void LoadFromPath(std::filesystem::path filePath);
	void SaveAnimation(std::filesystem::path savePath);

	glm::vec3 ManipulationGizmo(glm::vec3 position);

public:
	UJennyContext();
	~UJennyContext();

	bool Update(float deltaTime);
	void Render(float deltaTime);
};
