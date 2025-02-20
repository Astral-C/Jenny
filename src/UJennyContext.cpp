#include "UJennyContext.hpp"

#include "UPointSpriteManager.hpp"
#include "ImGuizmo.h"

#include "util/UUIUtil.hpp"

#include <J3D/J3DModelLoader.hpp>
#include <J3D/Data/J3DModelData.hpp>
#include <J3D/Material/J3DUniformBufferObject.hpp>
#include <J3D/Rendering/J3DLight.hpp>
#include <J3D/Data/J3DModelInstance.hpp>

#include <filesystem>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <bstream.h>
#include <optional>
#include <sys/types.h>
#include "ResUtil.hpp"

glm::vec3 UJennyContext::ManipulationGizmo(glm::vec3 position){
	glm::mat4 mtx = glm::translate(glm::identity<glm::mat4>(), position);
	glm::mat4 delta;

    ImGuizmo::Manipulate(&mCamera.GetViewMatrix()[0][0], &mCamera.GetProjectionMatrix()[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &mtx[0][0], &delta[0][0], NULL);
	return glm::vec3(delta[3]);
}

void SortFunc(J3D::Rendering::RenderPacketVector& packets) {
	std::vector<J3DRenderPacket> opaquePackets;
	std::vector<J3DRenderPacket> xluPackets;

	for (J3DRenderPacket packet : packets) {
		if ((packet.SortKey & 0x00800000) != 0) {
			opaquePackets.push_back(packet);
		}
		else {
			xluPackets.push_back(packet);
		}
	}

	std::sort(
		opaquePackets.begin(),
		opaquePackets.end(),
		[](const J3DRenderPacket& a, const J3DRenderPacket& b) -> bool {
			return a.Material->Name < b.Material->Name;
		}
	);
	std::sort(
		xluPackets.begin(),
		xluPackets.end(),
		[](const J3DRenderPacket& a, const J3DRenderPacket& b) -> bool {
			return a.Material->Name < b.Material->Name;
		}
	);

	packets.clear();

	for (J3DRenderPacket packet : opaquePackets) {
		packets.push_back(packet);
	}
	for (J3DRenderPacket packet : xluPackets) {
		packets.push_back(packet);
	}
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
	
	ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF((std::filesystem::current_path() / "res/NotoSansJP-Regular.otf").string().c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	mImage = Disk::Image::Create();
	
	if(Options.mRootDir != "" && (Options.mRootDir.extension() == ".gcm" || Options.mRootDir.extension() == ".iso")){
		bStream::CFileStream stream(Options.mRootDir, bStream::Endianess::Big, bStream::OpenMode::In);
		mImage->Load(&stream);
	}
	J3DUniformBufferObject::CreateUBO();
	J3D::Rendering::SetSortFunction(SortFunc);
	J3D::Picking::InitFramebuffer(1,1);
}

UJennyContext::~UJennyContext(){
	J3DUniformBufferObject::DestroyUBO();
	if(J3D::Picking::IsPickingEnabled()) J3D::Picking::DestroyFramebuffer();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glDeleteFramebuffers(1, &mFbo);
	glDeleteRenderbuffers(1, &mRbo);
	glDeleteTextures(1, &mViewTex);
	glDeleteTextures(1, &mPickTex);
}

bool UJennyContext::Update(float deltaTime) {
	if(Options.mRootChanged){
		if(Options.mRootDir != "" && std::filesystem::exists(Options.mRootDir) && (Options.mRootDir.extension() == ".gcm" || Options.mRootDir.extension() == ".iso")){
			bStream::CFileStream stream(Options.mRootDir, bStream::Endianess::Big, bStream::OpenMode::In);
			mImage->Load(&stream);
		}
		Options.mRootChanged = false;
	}
	mCamera.Update(deltaTime);
	return true;
}

void UJennyContext::Render(float deltaTime) {

	RenderMenuBar();
	
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoDockingInCentralNode;
	mMainDockSpaceID = ImGui::DockSpaceOverViewport(0, mainViewport, dockFlags);
	
	if(!bIsDockingSetUp){

		glGenFramebuffers(1, &mFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

		glGenRenderbuffers(1, &mRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, mRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);

		glGenTextures(1, &mViewTex);
		glGenTextures(1, &mPickTex);

		glBindTexture(GL_TEXTURE_2D, mViewTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, mPickTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 1280, 720, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mViewTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mPickTex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		ImGui::DockBuilderRemoveNode(mMainDockSpaceID); // clear any previous layout
		ImGui::DockBuilderAddNode(mMainDockSpaceID, dockFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(mMainDockSpaceID, mainViewport->Size);


		mDockNodeRightID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Left, 0.2f, nullptr, &mMainDockSpaceID);
		mDockNodeBottomRightID = ImGui::DockBuilderSplitNode(mDockNodeRightID, ImGuiDir_Down, 0.2f, nullptr, &mDockNodeRightID);

		ImGui::DockBuilderDockWindow("mainWindow", mDockNodeRightID);
		ImGui::DockBuilderDockWindow("detailWindow", mDockNodeBottomRightID);
		ImGui::DockBuilderDockWindow("viewportWindow", mMainDockSpaceID);

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
	ImGui::Begin("viewportWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

	glm::mat4 projection, view;
	projection = mCamera.GetProjectionMatrix();
	view = mCamera.GetViewMatrix();
	
	J3DUniformBufferObject::SetProjAndViewMatrices(projection, view);
	
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	ImVec2 winSize = ImGui::GetContentRegionAvail();
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	if(winSize.x != mPrevWinWidth || winSize.y != mPrevWinHeight){
		std::cout << "Regenerating textures..." << std::endl;
		glDeleteTextures(1, &mViewTex);
		glDeleteTextures(1, &mPickTex);
		glDeleteRenderbuffers(1, &mRbo);

		glGenRenderbuffers(1, &mRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, mRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (uint32_t)winSize.x, (uint32_t)winSize.y);

		glGenTextures(1, &mViewTex);
		glGenTextures(1, &mPickTex);

		glBindTexture(GL_TEXTURE_2D, mViewTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (uint32_t)winSize.x, (uint32_t)winSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, mPickTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, (uint32_t)winSize.x, (uint32_t)winSize.y, 0, GL_RED_INTEGER, GL_INT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mViewTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mPickTex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRbo);

		GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		J3D::Picking::ResizeFramebuffer((uint32_t)winSize.x, (uint32_t)winSize.y);
	
	}
	
	glViewport(0, 0, (uint32_t)winSize.x, (uint32_t)winSize.y);

	
	glClearColor(0.100f, 0.261f, 0.402f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//int32_t unused = 0;
	//glClearTexImage(mPickTex, 0, GL_RED_INTEGER, GL_INT, &unused);

	mPrevWinWidth = winSize.x;
	mPrevWinHeight = winSize.y;
	
	//if(!mSetLights) SetLights();
	J3DUniformBufferObject::SetProjAndViewMatrices(projection, view);
	mGrid.Render(mCamera.GetPosition(), mCamera.GetProjectionMatrix(), mCamera.GetViewMatrix());
	
	// Map Renderer
	mMapManager.RenderMap(deltaTime, &mCamera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	cursorPos = ImGui::GetCursorScreenPos();
	ImGui::Image(static_cast<uintptr_t>(mViewTex), winSize, {0.0f, 1.0f}, {1.0f, 0.0f});

	if(ImGui::IsItemClicked(0) && !ImGuizmo::IsOver()){
		ImVec2 mousePos = ImGui::GetMousePos();
		
		ImVec2 pickPos = {
			mousePos.x - cursorPos.x,
			winSize.y - (mousePos.y - cursorPos.y)
		};

		mMapManager.RenderMap(deltaTime, &mCamera, true);
		uint16_t id = std::get<0>(J3D::Picking::Query((uint32_t)pickPos.x,  (uint32_t)pickPos.y));

		// not a big fan of this

		for (auto gen : mMapManager.defaultGens.generators){
			if(gen->mModel != nullptr && gen->mModel->GetModelId() == id) selected = gen;
		}
		for (auto gen : mMapManager.initGens.generators){
			if(gen->mModel != nullptr && gen->mModel->GetModelId() == id) selected = gen;
		}
		for (auto gen : mMapManager.plantsGen.generators){
			if(gen->mModel != nullptr && gen->mModel->GetModelId() == id) selected = gen;
		}
		
		for(auto [loopName, loop] : mMapManager.loops){
			if(mMapManager.loopVisibility[loopName]){
				for(auto gen : loop.generators){
					if(gen->mModel != nullptr && gen->mModel->GetModelId() == id) selected = gen;
				}
			}
		}
		
		for(auto [loopName, loop] : mMapManager.nonloops){
			if(mMapManager.nonLoopVisibility[loopName]){
				for(auto gen : loop.generators){
					if(gen->mModel != nullptr && gen->mModel->GetModelId() == id) selected = gen;
				}
			}
		}

	}

	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::SetRect(cursorPos.x, cursorPos.y, winSize.x, winSize.y);
	
	//mBillboardManager.Draw(&mCamera);
	if(selected != nullptr && selected->mModel != nullptr){
		//selected->pos += ManipulationGizmo(selected->pos);
		ImGuizmo::Manipulate(&mCamera.GetViewMatrix()[0][0], &mCamera.GetProjectionMatrix()[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &selected->mTransform[0][0], NULL, NULL);
		selected->mModel->SetTransform(selected->mTransform);
	}
	
	ImGui::End();
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
			mMapManager.LoadMap(mImage, "tutorial");
		}

		if(ImGui::Selectable("forest", mMapSelected == "forest")){
			mMapSelected = "forest";
			mMapManager.LoadMap(mImage, "forest");
		}

		if(ImGui::Selectable("yakushima", mMapSelected == "yakushima")){
			mMapSelected = "yakushima";
			mMapManager.LoadMap(mImage, "yakushima");
		}
		
		if(ImGui::Selectable("last", mMapSelected == "last")){
			mMapSelected = "last";
			mMapManager.LoadMap(mImage, "last");
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

}

//todo: remove these

void UJennyContext::SaveAnimation(std::filesystem::path savePath){

}

void UJennyContext::LoadFromPath(std::filesystem::path filePath) {
    
}