#include "UMap.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

#include <fmt/format.h>
#include "bstream.h"

static std::map<std::string, std::shared_ptr<J3DModelData>> ModelCache;

PMapManager::PMapManager(){
	pathRenderer.Init();
}

PMapManager::~PMapManager(){
	ModelCache.clear();
}


void PMapManager::LoadEnemy(std::string modelDirName){
	std::filesystem::path modelPath = (Options.mRootDir / std::filesystem::path(std::format("files/enemy/data/{}/model.szs", modelDirName)));
	
	std::cout << modelPath.string() << std::endl;

	if(!std::filesystem::exists(modelPath)) return;

	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();
	
	bStream::CFileStream arcStream(modelPath.string(), bStream::Endianess::Big, bStream::OpenMode::In);
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
		ModelCache.insert({modelDirName, data});
		std::cout << "[JENNY]: Loaded model " << modelDirName << std::endl;
	}
}

void PMapManager::LoadTreasure(std::string modelArchiveName){
	std::filesystem::path modelPath = (Options.mRootDir / std::filesystem::path(std::format("files/user/Abe/Pellet/us/{}", modelArchiveName)));
	
	std::cout << modelPath.string() << std::endl;

	if(!std::filesystem::exists(modelPath)) return;

	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();
	
	bStream::CFileStream arcStream(modelPath.string(), bStream::Endianess::Big, bStream::OpenMode::In);
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
		ModelCache.insert({modelArchiveName, data});
		std::cout << "[JENNY]: Loaded model " << modelArchiveName << std::endl;
	}
}

void PMapManager::LoadKandoModel(std::string modelDirName, std::string modelName, std::string cacheName, std::string archiveName=""){
	std::filesystem::path modelPath = (Options.mRootDir / std::filesystem::path(std::format("files/user/Kando/{0}/{1}.szs", modelDirName, archiveName == "" ? "arc" : archiveName)));
	
	std::cout << modelPath.string() << std::endl;

	std::shared_ptr<Archive::Rarc> arc = Archive::Rarc::Create();
	
	bStream::CFileStream arcStream(modelPath.string(), bStream::Endianess::Big, bStream::OpenMode::In);
	if(!arc->Load(&arcStream)){
		return;
	}

	std::shared_ptr<Archive::File> bmdModel = arc->GetFile(modelName);

	if(bmdModel != nullptr){
		J3DModelLoader Loader;
		bStream::CMemoryStream modelStream((uint8_t*)bmdModel->GetData(), bmdModel->GetSize(), bStream::Endianess::Big, bStream::OpenMode::In);
				
		auto data = std::make_shared<J3DModelData>();
		data = Loader.Load(&modelStream, NULL);
		ModelCache.insert({cacheName, data});
		std::cout << "[JENNY]: Loaded model " << modelName << std::endl;
	}
}

void PMapManager::LoadAssets(){

	LoadKandoModel("onyon", "goal.bmd", "ONYN");
	LoadKandoModel("ufo", "ufo.bmd", "SHIP");
	
	LoadKandoModel("objects/plants", "model.bmd", "PLANT");

	LoadKandoModel("objects/dungeon_hole", "dungeon_hole.bmd", "CAVE");

	LoadKandoModel("objects/downfloor", "down_floor_1.bmd", "DWFL0");
	LoadKandoModel("objects/downfloor", "down_floor_2.bmd", "DWFL1");
	LoadKandoModel("objects/downfloor", "down_floor_3.bmd", "DWFL2");

	LoadKandoModel("objects/weed", "stone1.bmd", "WEED0");
	LoadKandoModel("objects/weed", "stone2.bmd", "WEED1");	
	LoadKandoModel("objects/weed", "stone3.bmd", "WEED2");
	LoadKandoModel("objects/weed", "weed1.bmd", "WEED3");

	LoadKandoModel("bridge/l_bridge", "l_bridge.bmd", "BRDG_LONG");
	LoadKandoModel("bridge/s_bridge", "s_bridge.bmd", "BRDG_SHORT");
	LoadKandoModel("bridge/slope_u", "slope_u.bmd", "BRDG_SLOPE");

	LoadKandoModel("objects/gates", "gate_soft.bmd", "GATE", "gate-arc");
	LoadKandoModel("objects/gates", "e-gate.bmd", "GATE_ELEC", "e-gate-arc");

	LoadKandoModel("objects/barrel", "model.bmd", "BARREL", "arc");

	for(auto dir : std::filesystem::directory_iterator(Options.mRootDir / std::filesystem::path("files/enemy/data/"))){
		std::cout << "Loading enemy Model " << dir << std::endl;
		LoadEnemy(dir.path().filename().string());
	}

	for(auto dir : std::filesystem::directory_iterator(Options.mRootDir / std::filesystem::path("files/user/Abe/Pellet/us/"))){
		std::cout << "Loading treasure Model " << dir << std::endl;
		if(dir.path().extension() == ".szs") LoadTreasure(dir.path().filename().string());
	}

	loadedAssets = true;

}

void PMapManager::LoadMap(std::string mapName){
	if(!loadedAssets) LoadAssets();

	loops.clear();
	nonloops.clear();

	loopVisibility.clear();
	nonLoopVisibility.clear();

	std::filesystem::path mapPath = (Options.mRootDir / std::filesystem::path(std::format("files/user/Kando/map/{}/", mapName)));
	std::filesystem::path mapGenPath = (Options.mRootDir / std::filesystem::path(std::format("files/user/Abe/map/{}/", mapName)));
	
	GCarchive mapArchive;
	GCResourceManager.LoadArchive((mapPath / "arc.szs").string().c_str(), &mapArchive);

	GCarcfile* mapModel = GCResourceManager.GetFile(&mapArchive, "model.bmd");

	if(mapModel != nullptr){
		J3DModelLoader Loader;
		bStream::CMemoryStream modelStream((uint8_t*)mapModel->data, mapModel->size, bStream::Endianess::Big, bStream::OpenMode::In);
				
		auto data = std::make_shared<J3DModelData>();
		data = Loader.Load(&modelStream, NULL);

		if(ModelCache.contains("map")){
			ModelCache["map"] = data;
		} else {
			ModelCache.insert({"map", data});
		}

		std::cout << "[JENNY]: Loaded map model" << std::endl;
	}

	gcFreeArchive(&mapArchive);


	// Set up lights from lights.ini?


	pathRenderer.mPaths.clear();

	// load up generators
	PMapManager::ParseGens(mapGenPath);
}

void PMapManager::RenderUI() {

}

void PMapManager::ParseGens(std::filesystem::path genPath){
	//bruh
	std::filesystem::path defaultGenPath = genPath /  "defaultgen.txt";
	std::filesystem::path initGenPath = genPath /  "initgen.txt";
	std::filesystem::path plantsGenPath = genPath /  "plantsgen.txt";

	
	defaultGens = PGenParser::ParseGenFile(defaultGenPath);
	initGens = PGenParser::ParseGenFile(initGenPath);
	plantsGen = PGenParser::ParseGenFile(plantsGenPath);

	std::filesystem::path routePath = genPath / "route.txt";
	routes = PGenParser::ParseRouteFile(routePath);

	for(auto dir : std::filesystem::directory_iterator(genPath / "loop")){
		std::cout << "Loading loop gens " << dir << std::endl;
		if(dir.path().extension() == ".txt"){
			loops.insert({dir.path().stem().string(), PGenParser::ParseGenFile(dir.path())});
			loopVisibility.insert({dir.path().stem().string(), false});
		}
	}

	for(auto dir : std::filesystem::directory_iterator(genPath / "nonloop")){
		std::cout << "Loading nonloop gens " << dir << std::endl;
		if(dir.path().extension() == ".txt"){
			nonloops.insert({dir.path().stem().string(), PGenParser::ParseGenFile(dir.path())});
			nonLoopVisibility.insert({dir.path().stem().string(), false});
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
	
	std::cout << "Pause" << std::endl;
}

void PMapManager::RenderGen(std::shared_ptr<PGenerator> gen, float dt){
	glm::mat4 transform = glm::identity<glm::mat4>();
	transform = glm::translate(transform, gen->pos);
	std::shared_ptr<J3DModelInstance> model;
	switch (gen->entityType)
	{
	case ONYN: {
			transform = glm::rotate(transform, glm::radians(static_pointer_cast<POnion>(gen->genData)->rotation.x), glm::vec3(1,0,0));
			transform = glm::rotate(transform, glm::radians(static_pointer_cast<POnion>(gen->genData)->rotation.y), glm::vec3(0,1,0));
			transform = glm::rotate(transform, glm::radians(static_pointer_cast<POnion>(gen->genData)->rotation.z), glm::vec3(0,0,1));
			POnion* onion = static_pointer_cast<POnion>(gen->genData).get();
			if(onion->index == 4){
				model = ModelCache["SHIP"];
			} else {
				model = ModelCache["ONYN"];
			}
		}
		break;
	
	case PLNT:
		model = ModelCache["PLANT"];
		break;
	case WEED: {
			PWeed* weed = static_pointer_cast<PWeed>(gen->genData).get();
			if(weed->type != 1){ //TODO: pick the correct rock model?
				model = ModelCache["WEED0"];
			} else {
				model = ModelCache["WEED3"];
			}
		}
		break;
	case CAVE:
		model = ModelCache["CAVE"];
		break;
	case BARL:
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PBarrel>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PBarrel>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PBarrel>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		model = ModelCache["BARREL"];
		break;
	case PELT:
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PPelt>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PPelt>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PPelt>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		model = ModelCache[TreasureInternalName[static_pointer_cast<PPelt>(gen->genData)->peltID]];
		break;
	case GATE:
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		model = ModelCache["GATE"];
		break;
	case DGAT:
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.x), glm::vec3(1,0,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.y), glm::vec3(0,1,0));
		transform = glm::rotate(transform, glm::radians(static_pointer_cast<PGate>(gen->genData)->rotation.z), glm::vec3(0,0,1));
		model = ModelCache["GATE_ELEC"];
		break;
	case BRDG: {
			PBridge* bridge = static_pointer_cast<PBridge>(gen->genData).get();
			transform = glm::rotate(transform, glm::radians(bridge->rotation.x), glm::vec3(1,0,0));
			transform = glm::rotate(transform, glm::radians(bridge->rotation.y), glm::vec3(0,1,0));
			transform = glm::rotate(transform, glm::radians(bridge->rotation.z), glm::vec3(0,0,1));

			switch(bridge->type){
				case 0:
					model = ModelCache["BRDG_SHORT"];
					break;
				case 1:
					model = ModelCache["BRDG_SLOPE"];
					break;
				case 2:
					model = ModelCache["BRDG_LONG"];
					break;
			}
		}
		break;
	case TEKI: {
			PTeki* teki = static_pointer_cast<PTeki>(gen->genData).get();
			transform = glm::rotate(transform, glm::radians(teki->direction), glm::vec3(0,1,0));
			model = ModelCache[EnemyInternalName[teki->id]];
		}
		break;
	case DWFL:{
			PDownfloor* dwfl = static_pointer_cast<PDownfloor>(gen->genData).get();
			transform = glm::rotate(transform, glm::radians(dwfl->rotation.x), glm::vec3(1,0,0));
			transform = glm::rotate(transform, glm::radians(dwfl->rotation.y), glm::vec3(0,1,0));
			transform = glm::rotate(transform, glm::radians(dwfl->rotation.z), glm::vec3(0,0,1));

			model = ModelCache[std::format("DWFL{}", dwfl->dwflType)];
		}
		break;
	default:
		model = nullptr;
		break;
	}
	if(model != nullptr){
		J3DUniformBufferObject::SetEnvelopeMatrices(model->GetRestPose().data(), model->GetRestPose().size());		
		J3DUniformBufferObject::SetModelMatrix(&transform);
		model->Render(dt);
	}
}

void PMapManager::RenderMap(float dt, USceneCamera* camera){
	J3DUniformBufferObject::SetProjAndViewMatrices(projection, view);
	if(ModelCache.contains("map")){
		glm::mat4 map = glm::identity<glm::mat4>();
		//instance map
		//ModelCache["map"]->Render(dt);
	}

	for (auto gen : defaultGens.generators){
		RenderGen(gen, dt);
	}
	for (auto gen : initGens.generators){
		RenderGen(gen, dt);
	}
	for (auto gen : plantsGen.generators){
		RenderGen(gen, dt);
	}

	for(auto [loopName, loop] : loops){
		if(loopVisibility[loopName]){
			for(auto gen : loop.generators){
				RenderGen(gen, dt);
			}
		}
	}

	for(auto [loopName, loop] : nonloops){
		if(nonLoopVisibility[loopName]){
			for(auto gen : loop.generators){
				RenderGen(gen, dt);
			}
		}
	}

	pathRenderer.Draw(camera);
}