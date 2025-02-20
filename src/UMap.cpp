#include "UMap.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

#include <J3D/Animation/J3DColorAnimationInstance.hpp>
#include <J3D/Animation/J3DTexIndexAnimationInstance.hpp>
#include <J3D/Animation/J3DTexMatrixAnimationInstance.hpp>
#include <J3D/Animation/J3DJointAnimationInstance.hpp>
#include <J3D/Animation/J3DJointFullAnimationInstance.hpp>
#include <J3D/Animation/J3DVisibilityAnimationInstance.hpp>
#include <J3D/Animation/J3DAnimationLoader.hpp>

#include <format>
#include "bstream.h"

static std::map<std::string, std::shared_ptr<J3DModelData>> ModelCache;

PMapManager::PMapManager(){
	pathRenderer.Init();
}

PMapManager::~PMapManager(){
	ModelCache.clear();
}


void PMapManager::LoadEnemy(std::shared_ptr<Disk::Folder> enemyDir){

	if(enemyDir->GetFile("model.szs") == nullptr) return;
	auto modelFile = enemyDir->GetFile("model.szs");

	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();
	
	bStream::CMemoryStream arcStream(modelFile->GetData(), modelFile->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
	if(!arc->Load(&arcStream)){
		return;
	}

	std::shared_ptr<Archive::File> bmdModel = nullptr;
	for(auto file : arc->GetRoot()->GetFiles()){
		if(std::filesystem::path(file->GetName()).extension() == ".bmd"){
			bmdModel = file;
			break;
		}
	} 

	if(bmdModel != nullptr){
		J3DModelLoader Loader;
		bStream::CMemoryStream modelStream((uint8_t*)bmdModel->GetData(), bmdModel->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
				
		auto data = std::make_shared<J3DModelData>();
		data = Loader.Load(&modelStream, NULL);
		ModelCache.insert({std::filesystem::path(enemyDir->GetName()).stem().string(), data});
		std::cout << "[JENNY]: Loaded model " << enemyDir->GetName() << std::endl;
	}
}

void PMapManager::LoadTreasure(std::shared_ptr<Disk::File> treasureArc){
	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();
	
	bStream::CMemoryStream arcStream(treasureArc->GetData(), treasureArc->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
	if(!arc->Load(&arcStream)){
		return;
	}

	std::shared_ptr<Archive::File> bmdModel = nullptr;
	for(auto file : arc->GetRoot()->GetFiles()){
		if(std::filesystem::path(file->GetName()).extension() == ".bmd"){
			bmdModel = file;
			break;
		}
	} 

	if(bmdModel != nullptr){
		J3DModelLoader Loader;
		bStream::CMemoryStream modelStream((uint8_t*)bmdModel->GetData(), bmdModel->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
				
		auto data = std::make_shared<J3DModelData>();
		data = Loader.Load(&modelStream, NULL);
		ModelCache.insert({std::filesystem::path(treasureArc->GetName()).stem().string(), data});
		std::cout << "[JENNY]: Loaded model " << treasureArc->GetName() << std::endl;
	}
}

void PMapManager::LoadKandoModel(std::shared_ptr<Disk::Folder> kandoDir, std::string modelDirName, std::string modelName, std::string cacheName, std::string archiveName=""){
	std::filesystem::path modelPath = std::filesystem::path(std::format("{0}/{1}.szs", modelDirName, archiveName == "" ? "arc" : archiveName));

	if(kandoDir->GetFile(modelPath) == nullptr) return;
	
	auto modelFile = kandoDir->GetFile(modelPath);
	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();
	
	bStream::CMemoryStream arcStream(modelFile->GetData(), modelFile->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
	if(!arc->Load(&arcStream)){
		return;
	}

	std::shared_ptr<Archive::File> bmdModel = arc->GetFile(modelName);

	if(bmdModel != nullptr){
		J3DModelLoader Loader;
		bStream::CMemoryStream modelStream((uint8_t*)bmdModel->GetData(), bmdModel->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
				
		auto data = std::make_shared<J3DModelData>();
		data = Loader.Load(&modelStream, NULL);
		ModelCache.insert({std::filesystem::path(cacheName).stem().string(), data});
		std::cout << "[JENNY]: Loaded model " << modelName << std::endl;
	}
}

void PMapManager::LoadAssets(std::shared_ptr<Disk::Image> image){

	auto kandoDir = image->GetFolder("files/user/Kando");
	auto treasureDir = image->GetFolder("files/user/Abe/Pellet/us");
	auto enemyModelDir = image->GetFolder("files/enemy/data");

	LoadKandoModel(kandoDir, "onyon", "goal.bmd", "ONYN");
	LoadKandoModel(kandoDir, "ufo", "ufo.bmd", "SHIP");
	LoadKandoModel(kandoDir, "objects/plants", "model.bmd", "PLANT");
	LoadKandoModel(kandoDir, "objects/dungeon_hole", "dungeon_hole.bmd", "CAVE");
	LoadKandoModel(kandoDir, "objects/downfloor", "down_floor_1.bmd", "DWFL0");
	LoadKandoModel(kandoDir, "objects/downfloor", "down_floor_2.bmd", "DWFL1");
	LoadKandoModel(kandoDir, "objects/downfloor", "down_floor_3.bmd", "DWFL2");
	LoadKandoModel(kandoDir, "objects/weed", "stone1.bmd", "WEED0");
	LoadKandoModel(kandoDir, "objects/weed", "stone2.bmd", "WEED1");	
	LoadKandoModel(kandoDir, "objects/weed", "stone3.bmd", "WEED2");
	LoadKandoModel(kandoDir, "objects/weed", "weed1.bmd", "WEED3");
	LoadKandoModel(kandoDir, "bridge/l_bridge", "l_bridge.bmd", "BRDG_LONG");
	LoadKandoModel(kandoDir, "bridge/s_bridge", "s_bridge.bmd", "BRDG_SHORT");
	LoadKandoModel(kandoDir, "bridge/slope_u", "slope_u.bmd", "BRDG_SLOPE");
	LoadKandoModel(kandoDir, "objects/gates", "gate_soft.bmd", "GATE", "gate-arc");
	LoadKandoModel(kandoDir, "objects/gates", "e-gate.bmd", "GATE_ELEC", "e-gate-arc");
	LoadKandoModel(kandoDir, "objects/barrel", "model.bmd", "BARREL", "arc");

	for(auto dir : enemyModelDir->GetSubdirectories()){
		LoadEnemy(dir);
	}

	for(auto file : treasureDir->GetFiles()){
		if(std::filesystem::path(file->GetName()).extension() == ".szs") LoadTreasure(file);
	}

	loadedAssets = true;

}

void PMapManager::LoadMap(std::shared_ptr<Disk::Image> image, std::string mapName){
	if(!loadedAssets) LoadAssets(image);

	loops.clear();
	nonloops.clear();

	loopVisibility.clear();
	nonLoopVisibility.clear();

	std::filesystem::path mapPath = std::filesystem::path(std::format("files/user/Kando/map/{}/arc.szs", mapName));
	std::filesystem::path mapGenPath = std::filesystem::path(std::format("files/user/Abe/map/{}/", mapName));
	
	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();

	auto mapArcFile = image->GetFile(mapPath); 
	
	bStream::CMemoryStream arcStream(mapArcFile->GetData(), mapArcFile->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
	if(!arc->Load(&arcStream)){
		std::cout << "Failed to load map model " << mapPath << std::endl;
		return;
	}

	std::shared_ptr<Archive::File> mapModel = arc->GetFile("model.bmd");

	if(mapModel != nullptr){
		J3DModelLoader Loader;
		bStream::CMemoryStream modelStream((uint8_t*)mapModel->GetData(), mapModel->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
				
		auto data = std::make_shared<J3DModelData>();
		data = Loader.Load(&modelStream, NULL);

		mMapModel = data->CreateInstance();

		std::cout << "[JENNY]: Loaded map model" << std::endl;
	}

	// Set up lights from lights.ini?


	pathRenderer.mPaths.clear();

	// load up generators
	PMapManager::ParseGens(image, mapGenPath);
}

void PMapManager::RenderUI() {

}

void PMapManager::ParseGens(std::shared_ptr<Disk::Image> image, std::filesystem::path genPath){
	//bruh
	std::filesystem::path defaultGenPath = genPath /  "defaultgen.txt";
	std::filesystem::path initGenPath = genPath /  "initgen.txt";
	std::filesystem::path plantsGenPath = genPath /  "plantsgen.txt";

	
	defaultGens = PGenParser::ParseGenFile(image->GetFile(defaultGenPath));
	initGens = PGenParser::ParseGenFile(image->GetFile(initGenPath));
	plantsGen = PGenParser::ParseGenFile(image->GetFile(plantsGenPath));

	std::filesystem::path routePath = genPath / "route.txt";
	routes = PGenParser::ParseRouteFile(image->GetFile(routePath));

	for(auto file : image->GetFolder(genPath / "loop")->GetFiles()){
		std::cout << "Loading loop gens " << file << std::endl;
		if(std::filesystem::path(file->GetName()).extension() == ".txt"){
			loops.insert({std::filesystem::path(file->GetName()).stem().string(), PGenParser::ParseGenFile(file)});
			loopVisibility.insert({std::filesystem::path(file->GetName()), false});
		}
	}

	for(auto file : image->GetFolder(genPath / "nonloop")->GetFiles()){
		std::cout << "Loading nonloop gens " << file << std::endl;
		if(std::filesystem::path(file->GetName()).extension() == ".txt"){
			nonloops.insert({std::filesystem::path(file->GetName()).stem().string(), PGenParser::ParseGenFile(file)});
			nonLoopVisibility.insert({std::filesystem::path(file->GetName()), false});
		}
	}
	
	// setup path rendering from routes

	for (auto& waypoint : routes.waypoints){
		std::vector<CPathPoint> waypointLinks;
		for(auto& linked : waypoint->links){
			waypointLinks.push_back({waypoint->position, glm::vec4(0.8,0.5,0.95,1.0), 12800});
			std::shared_ptr<PWaypoint> waypoint_linked = linked.lock();
			if(waypoint_linked != nullptr){
				waypointLinks.push_back({
					waypoint_linked->position,
					glm::vec4(0.8,0.5,0.95,1.0),
					12800
				});
			}
		}
		pathRenderer.mPaths.push_back(waypointLinks);
	}
		
	pathRenderer.UpdateData();
	
	for (auto gen : defaultGens.generators){
		GenLoadRenderable(gen);
	}
	for (auto gen : initGens.generators){
		GenLoadRenderable(gen);
	}
	for (auto gen : plantsGen.generators){
		GenLoadRenderable(gen);
	}
	
	for(auto [loopName, loop] : loops){
		if(loopVisibility[loopName]){
			for(auto gen : loop.generators){
				GenLoadRenderable(gen);
			}
		}
	}
	
	for(auto [loopName, loop] : nonloops){
		if(nonLoopVisibility[loopName]){
			for(auto gen : loop.generators){
				GenLoadRenderable(gen);
			}
		}
	}

	std::cout << "Pause" << std::endl;
}

void PMapManager::GenLoadRenderable(std::shared_ptr<PGenerator> gen){
	gen->mTransform = glm::identity<glm::mat4>();
	gen->mTransform = glm::translate(gen->mTransform, gen->pos);
	std::shared_ptr<J3DModelData> model = nullptr;
	switch (gen->entityType)
	{
	case ONYN: {
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<POnion>(gen->genData)->rotation.x), glm::vec3(1,0,0));
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<POnion>(gen->genData)->rotation.y), glm::vec3(0,1,0));
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<POnion>(gen->genData)->rotation.z), glm::vec3(0,0,1));
			POnion* onion = static_pointer_cast<POnion>(gen->genData).get();
			if(onion->index == 4){
				if(ModelCache.contains("SHIP")) model = ModelCache["SHIP"];
			} else {
				if(ModelCache.contains("ONYN")) model = ModelCache["ONYN"];
			}
		}
		break;
	
	case PLNT:
		if(ModelCache.contains("PLANT")) model = ModelCache["PLANT"];
		break;
	case WEED: {
			PWeed* weed = static_pointer_cast<PWeed>(gen->genData).get();
			if(weed->type != 1){ //TODO: pick the correct rock model?
				if(ModelCache.contains("WEED0")) model = ModelCache["WEED0"];
			} else {
				if(ModelCache.contains("WEED3")) model = ModelCache["WEED3"];
			}
		}
		break;
	case CAVE:
		if(ModelCache.contains("CAVE")) model = ModelCache["CAVE"];
		break;
	case BARL:
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PBarrel>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PBarrel>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PBarrel>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		if(ModelCache.contains("BARREL")) model = ModelCache["BARREL"];
		break;
	case PELT:
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PPelt>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PPelt>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PPelt>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		if(ModelCache.contains(TreasureInternalName[static_pointer_cast<PPelt>(gen->genData)->peltID])) model = ModelCache[TreasureInternalName[static_pointer_cast<PPelt>(gen->genData)->peltID]];
		break;
	case GATE:
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		if(ModelCache.contains("GATE")) model = ModelCache["GATE"];
		break;
	case DGAT:
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		gen->mTransform = glm::rotate(gen->mTransform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		if(ModelCache.contains("GATE_ELEC")) model = ModelCache["GATE_ELEC"];
		break;
	case BRDG: {
			PBridge* bridge = static_pointer_cast<PBridge>(gen->genData).get();
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(bridge->rotation.x), glm::vec3(1,0,0));
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(bridge->rotation.y), glm::vec3(0,1,0));
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(bridge->rotation.z), glm::vec3(0,0,1));

			switch(bridge->type){
				case 0:
					if(ModelCache.contains("BRDG_SHORT")) model = ModelCache["BRDG_SHORT"];
					break;
				case 1:
					if(ModelCache.contains("BRDG_SLOPE")) model = ModelCache["BRDG_SLOPE"];
					break;
				case 2:
					if(ModelCache.contains("BRDG_LONG")) model = ModelCache["BRDG_LONG"];
					break;
			}
		}
		break;
	case TEKI: {
			PTeki* teki = static_pointer_cast<PTeki>(gen->genData).get();
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(teki->direction), glm::vec3(0,1,0));
			if(ModelCache.contains(EnemyInternalName[teki->id])) model = ModelCache[EnemyInternalName[teki->id]];
		}
		break;
	case DWFL:{
			PDownfloor* dwfl = static_pointer_cast<PDownfloor>(gen->genData).get();
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(dwfl->rotation.x), glm::vec3(1,0,0));
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(dwfl->rotation.y), glm::vec3(0,1,0));
			gen->mTransform = glm::rotate(gen->mTransform, glm::radians(dwfl->rotation.z), glm::vec3(0,0,1));

			if(ModelCache.contains(std::format("DWFL{}", dwfl->dwflType))) model = ModelCache[std::format("DWFL{}", dwfl->dwflType)];
		}
		break;
	default:
		model = nullptr;
		break;
	}

	if(model != nullptr){
		gen->mModel = model->CreateInstance();
		gen->mModel->SetTransform(gen->mTransform);
	}
}

void PMapManager::RenderMap(float dt, USceneCamera* camera, bool picking){
	std::vector<std::shared_ptr<J3DModelInstance>> renderables;
	renderables.reserve(defaultGens.generators.size() + initGens.generators.size() + plantsGen.generators.size() + loops.size() + nonloops.size() + 1);
	
	for (auto gen : defaultGens.generators){
		if(gen->mModel != nullptr) renderables.push_back(gen->mModel);
	}
	for (auto gen : initGens.generators){
		if(gen->mModel != nullptr) renderables.push_back(gen->mModel);
	}
	for (auto gen : plantsGen.generators){
		if(gen->mModel != nullptr) renderables.push_back(gen->mModel);
	}
	
	for(auto [loopName, loop] : loops){
		if(loopVisibility[loopName]){
			for(auto gen : loop.generators){
				if(gen->mModel != nullptr) renderables.push_back(gen->mModel);
			}
		}
	}
	
	for(auto [loopName, loop] : nonloops){
		if(nonLoopVisibility[loopName]){
			for(auto gen : loop.generators){
				if(gen->mModel != nullptr) renderables.push_back(gen->mModel);
			}
		}
	}


	if(mMapModel != nullptr){
		renderables.push_back(mMapModel);
	}
	
	auto proj = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	auto packets = J3D::Rendering::SortPackets(renderables, camera->GetPosition());
	
	pathRenderer.Draw(camera);

	if(picking){
		J3D::Picking::RenderPickingScene(view, proj, packets);
	} else {
		J3D::Rendering::Render(dt, proj, view, packets);
	}
}