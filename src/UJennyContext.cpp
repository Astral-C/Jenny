#include "UJennyContext.hpp"

#include "UPointSpriteManager.hpp"
#include "ImGuizmo.h"

#include "util/UUIUtil.hpp"

#include <J3D/J3DModelLoader.hpp>
#include <J3D/J3DModelData.hpp>
#include <J3D/J3DUniformBufferObject.hpp>
#include <J3D/J3DLight.hpp>
#include <J3D/J3DModelInstance.hpp>

#include <bits/fs_path.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <bstream.h>
#include <optional>
#include <sys/types.h>
#include "fmt/core.h"
#include "ResUtil.hpp"

glm::vec3 UJennyContext::ManipulationGizmo(glm::vec3 position){
	glm::mat4 mtx = glm::translate(glm::identity<glm::mat4>(), position);
	glm::mat4 delta;

    ImGuizmo::Manipulate(&mCamera.GetViewMatrix()[0][0], &mCamera.GetProjectionMatrix()[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &mtx[0][0], &delta[0][0], NULL);
	return glm::vec3(delta[3]);
}

UJennyContext::UJennyContext(){
	Options.LoadOptions();
	mGrid.Init();
	mBillboardManager.Init(128, 2);

	// TODO: figure out what point sprites are needed?
	mBillboardManager.SetBillboardTexture(std::filesystem::current_path() / "res/camera.png", 0);
	mBillboardManager.SetBillboardTexture(std::filesystem::current_path() / "res/target.png", 1);

	mBillboardManager.mBillboards.push_back(UPointSprite());
	mBillboardManager.mBillboards.push_back(UPointSprite());

	mBillboardManager.mBillboards[0].Texture = 0;
	mBillboardManager.mBillboards[0].SpriteSize = 204800;
	mBillboardManager.mBillboards[1].Texture = 1;
	mBillboardManager.mBillboards[1].SpriteSize = 204800;

	GCResourceManager.Init();

	ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF((std::filesystem::current_path() / "res/NotoSansJP-Regular.otf").string().c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

bool UJennyContext::Update(float deltaTime) {
	
	mCamera.Update(deltaTime);
	return true;
}

void UJennyContext::Render(float deltaTime) {

	RenderMenuBar();
	
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoDockingInCentralNode;
	mMainDockSpaceID = ImGui::DockSpaceOverViewport(mainViewport, dockFlags);
	
	if(!bIsDockingSetUp){
		ImGui::DockBuilderRemoveNode(mMainDockSpaceID); // clear any previous layout
		ImGui::DockBuilderAddNode(mMainDockSpaceID, dockFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(mMainDockSpaceID, mainViewport->Size);


		mDockNodeRightID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Left, 0.2f, nullptr, &mMainDockSpaceID);
		mDockNodeBottomRightID = ImGui::DockBuilderSplitNode(mDockNodeRightID, ImGuiDir_Down, 0.2f, nullptr, &mDockNodeRightID);

		ImGui::DockBuilderDockWindow("mainWindow", mDockNodeRightID);
		ImGui::DockBuilderDockWindow("detailWindow", mDockNodeBottomRightID);

		ImGui::DockBuilderFinish(mMainDockSpaceID);
		bIsDockingSetUp = true;
	}


	ImGuiWindowClass mainWindowOverride;
	mainWindowOverride.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
	ImGui::SetNextWindowClass(&mainWindowOverride);
	
	ImGui::Begin("mainWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	ImGui::Text("Generators");
	ImGui::Separator();

	if(ImGui::TreeNode("Default Gen")){
		for(auto gen : mMapManager.defaultGens.generators){
			ImGui::TextColored((selected == gen ? ImVec4(0.0,1.0,0.0,1.0) : ImVec4(1.0,1.0,1.0,1.0)), gen->GetName().c_str());
			if(ImGui::IsItemClicked(0)){
				selected = gen;
			}
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Init Gen")){
		for(auto gen : mMapManager.initGens.generators){
			ImGui::TextColored((selected == gen ? ImVec4(0.0,1.0,0.0,1.0) : ImVec4(1.0,1.0,1.0,1.0)), gen->GetName().c_str());
			if(ImGui::IsItemClicked(0)){
				selected = gen;
			}
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Plant Gen")){
		for(auto gen : mMapManager.plantsGen.generators){
			ImGui::TextColored((selected == gen ? ImVec4(0.0,1.0,0.0,1.0) : ImVec4(1.0,1.0,1.0,1.0)), gen->GetName().c_str());
			if(ImGui::IsItemClicked(0)){
				selected = gen;
			}
		}
		ImGui::TreePop();
	}

	for(auto [loopName, loop] : mMapManager.loops){
		ImGui::Checkbox(std::format("##renderLoop{}", loopName).c_str(), &mMapManager.loopVisibility[loopName]);
		ImGui::SameLine();
		if(ImGui::TreeNode(std::format("Loop {}", loopName).c_str())){
			for(auto gen : loop.generators){
				ImGui::TextColored((selected == gen ? ImVec4(0.0,1.0,0.0,1.0) : ImVec4(1.0,1.0,1.0,1.0)), gen->GetName().c_str());
				if(ImGui::IsItemClicked(0)){
					selected = gen;
				}
			}
			ImGui::TreePop();
		}
	}

	for(auto [loopName, loop] : mMapManager.nonloops){
		ImGui::Checkbox(std::format("##renderNonLoop{}", loopName).c_str(), &mMapManager.nonLoopVisibility[loopName]);
		ImGui::SameLine();
		if(ImGui::TreeNode(std::format("NonLoop {}", loopName).c_str())){
			for(auto gen : loop.generators){
				ImGui::TextColored((selected == gen ? ImVec4(0.0,1.0,0.0,1.0) : ImVec4(1.0,1.0,1.0,1.0)), gen->GetName().c_str());
				if(ImGui::IsItemClicked(0)){
					selected = gen;
				}
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();

	ImGui::SetNextWindowClass(&mainWindowOverride);

	ImGui::Begin("detailWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	ImGui::Text("Gen Settings");

	if(selected != nullptr){
		ImGui::Text("%d", selected->entityType);
	}

	ImGui::Separator();
	ImGui::End();

	ImGui::SetNextWindowClass(&mainWindowOverride);

	/*
	ImGui::Begin("zoneWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		ImGui::Text("???");
		ImGui::SameLine(ImGui::GetWindowWidth()-40);
		if(ImGui::Button("Hide")) mShowZones = false;
		ImGui::Separator();
		// render map manager ui
	ImGui::End();
	*/

	glm::mat4 projection, view;
	projection = mCamera.GetProjectionMatrix();
	view = mCamera.GetViewMatrix();

	SetLights();
	
	J3DUniformBufferObject::SetProjAndViewMatrices(&projection, &view);
	
	//Render Models here
	mGrid.Render(mCamera.GetPosition(), mCamera.GetProjectionMatrix(), mCamera.GetViewMatrix());
	
	// Map Renderer
	mMapManager.RenderMap(deltaTime, &mCamera);

	ImGuizmo::BeginFrame();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	//mBillboardManager.Draw(&mCamera);
	if(selected != nullptr){
		selected->pos += ManipulationGizmo(selected->pos);
	}

}

void UJennyContext::RenderMainWindow(float deltaTime) {


}

void UJennyContext::RenderMenuBar() {
	bOptionsOpen = false;
	bIsMapSelectDialogOpen = false;

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Select Map...")) {
			//TODO: Open zone..
			bIsMapSelectDialogOpen = true;
		}
		if (ImGui::MenuItem("Save Map...")) {
			//SaveModelCB();
		}

		ImGui::Separator();
		ImGui::MenuItem("Close");

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		if(ImGui::MenuItem("Settings")){
			bOptionsOpen = true;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("About")) {
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (bIsMapSelectDialogOpen) {
		ImGui::OpenPopup("MapSelect");
	}


	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	
	if (ImGui::BeginPopupModal("MapSelect", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text("Map Select");
		
		if(ImGui::Selectable("tutorial", mMapSelected == "tutorial")){
			mMapSelected = "tutorial";
			mMapManager.LoadMap("tutorial");
		}

		if(ImGui::Selectable("forest", mMapSelected == "forest")){
			mMapSelected = "forest";
			mMapManager.LoadMap("forest");
		}

		if(ImGui::Selectable("yakushima", mMapSelected == "yakushima")){
			mMapSelected = "yakushima";
			mMapManager.LoadMap("yakushima");
		}
		
		if(ImGui::Selectable("last", mMapSelected == "last")){
			mMapSelected = "last";
			mMapManager.LoadMap("last");
		}

		if(ImGui::Button("Close")){
			ImGui::CloseCurrentPopup();
		}
		
		ImGui::EndPopup();
	}

	if(bOptionsOpen){
		ImGui::OpenPopup("Options");
	}

	Options.RenderOptionMenu();
}

void UJennyContext::OpenModelCB() {
}

void UJennyContext::SaveModelCB() {
}

void UJennyContext::SetLights() {

	J3DLight lights[8];

	for (int i = 0; i < 8; i++){
		lights[i].Position = glm::vec4(0.0, 10.0, 0.0, 1);
		lights[i].AngleAtten = glm::vec4(0.5, 0.5, 0.5, 1);
		lights[i].DistAtten = glm::vec4(0.5, 0.5, 0.5, 1);
		lights[i].Direction = glm::vec4(0.0, 0.0, 0.0, 1);
		lights[i].Color = glm::vec4(1, 1, 1, 1);
	}

	J3DUniformBufferObject::SetLights(lights);
}

//todo: remove these

void UJennyContext::SaveAnimation(std::filesystem::path savePath){

}

void UJennyContext::LoadFromPath(std::filesystem::path filePath) {
    
}