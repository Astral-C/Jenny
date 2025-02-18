#define GLM_ENABLE_EXPERIMENTAL
#include <io/GenParser.hpp>
#include <GenUtil.hpp>
#include <algorithm>
#include <iostream>
#include <stack>

namespace PGenParser {

    bool WriteGenFile(std::filesystem::path genPath){
        // write gen file
        return true;
    }


    /*
     * 
     * This parser is SO absolutely TERRIBLE I want to rewrite the whole thing.
     * 
     * Just wanted to get SOMETHING loaded to see in the 3D view but this is so bad.
     * 
    */


    PGenCollection ParseGenFile(std::filesystem::path genPath){
        PGenCollection gens;
        std::ifstream genTxt(genPath);


        if(genTxt){
            
            std::string genTxtAsStr((std::istreambuf_iterator<char>(genTxt)), (std::istreambuf_iterator<char>()));
            
            // For some reason iconv can't properly convert all the text files we throw at it, not sure why.
            //genTxtAsStr = LGenUtility::SjisToUtf8(genTxtAsStr);
            
            std::stringstream genFile(genTxtAsStr);

            std::string line;
            getline(genFile, line); // Consume the rest of this line
            
            genFile >> line;

            if(line.find("{v0.1}") != std::string::npos) gens.ver = VER1;
            if(line.find("{v0.3}") != std::string::npos) gens.ver = VER3;
            std::cout << "Read version " << line << " Version ID: " << gens.ver << std::endl;
            
            // skip rest of line
            getline(genFile, line);
            
            genFile >> gens.startPos.z;
            genFile >> gens.startPos.y;
            genFile >> gens.startPos.x;
            getline(genFile, line);

            genFile >> gens.startDir;
            getline(genFile, line);

            genFile >> gens.numGen;
            std::cout << "Start Pos " << gens.startPos.x << " " << gens.startPos.y << " " << gens.startPos.z << std::endl << "Start Dir: " << gens.startDir << std::endl << "Num of Generators: " << gens.numGen << std::endl;
            
            getline(genFile, line);

            for(int genIdx = 0; genIdx < gens.numGen; genIdx++){
                getline(genFile, line);
                getline(genFile, line);
            
                PGenerator gen;

                getline(genFile, line);
                if(line.find("{v0.1}") != std::string::npos) gen.ver = VER1;
                if(line.find("{v0.3}") != std::string::npos) gen.ver = VER3;

                //std::cout << "Read version " << line << " Version ID: " << gen.ver << std::endl;

                genFile >> gen.reserved;
                getline(genFile, line);
                
                genFile >> gen.respawnDays;
                getline(genFile, line);

                //genFile >> gen.
                getline(genFile, line); //args

                genFile >> gen.pos.x;
                genFile >> gen.pos.y;
                genFile >> gen.pos.z;
                getline(genFile, line);
                std::cout << "Pos " << gen.pos.x << " " << gen.pos.y << " " << gen.pos.z << std::endl;

                genFile >> gen.offset.x;
                genFile >> gen.offset.y;
                genFile >> gen.offset.z;
                getline(genFile, line);
                std::cout << "Offset " << gen.offset.x << " " << gen.offset.y << " " << gen.offset.z << std::endl;

                std::string entityType;
                
                genFile >> entityType;


                std::cout << "Entity Type is " << entityType << std::endl;

                if(entityType != "{teki}"){
                    getline(genFile, line);
                    getline(genFile, line);
                    std::string itemID;            

                    genFile >> itemID;
                    
                    if(PObjectTagStr.contains(itemID)){
                        gen.entityType = PObjectTagStr.at(itemID);
                    } else if(PObjectTagStr.contains(entityType)){
                        gen.entityType = PObjectTagStr.at(entityType);
                    } else {
                        gen.entityType = TAG_MAX;
                    }
                } else {
                    gen.entityType = TEKI;
                }


                switch (gen.entityType)
                {                
                case ONYN: {
                        getline(genFile, line);
                        std::cout << "Reading Onion..." << std::endl;
                        gen.genData = std::make_shared<POnion>();
                        genFile >> std::static_pointer_cast<POnion>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<POnion>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<POnion>(gen.genData)->rotation.z;
                        
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<POnion>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        std::string temp;
                        genFile >> temp;
                        std::static_pointer_cast<POnion>(gen.genData)->index = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp; 
                        std::static_pointer_cast<POnion>(gen.genData)->afterBoot = stoi(temp);
                        getline(genFile, line);

                        getline(genFile, line);

                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                    }
                    break;

                case WEED: {
                        getline(genFile, line);
                        std::cout << "Reading Weed..." << std::endl;
                        gen.genData = std::make_shared<PWeed>();
                        genFile >> std::static_pointer_cast<PWeed>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PWeed>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PWeed>(gen.genData)->rotation.z;
                        
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PWeed>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        std::string temp;
                        genFile >> temp;
                        std::static_pointer_cast<PWeed>(gen.genData)->weedAmount = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp; 
                        std::static_pointer_cast<PWeed>(gen.genData)->type = stoi(temp);
                        getline(genFile, line);

                        getline(genFile, line);

                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                    }
                    break;

                case DWFL: {
                        getline(genFile, line);
                        std::cout << "Reading Downfloor..." << std::endl;
                        gen.genData = std::make_shared<PDownfloor>();
                        genFile >> std::static_pointer_cast<PDownfloor>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PDownfloor>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PDownfloor>(gen.genData)->rotation.z;
                        
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PDownfloor>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        std::string temp;
                        genFile >> temp;
                        std::static_pointer_cast<PDownfloor>(gen.genData)->pressWeight = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp; 
                        std::static_pointer_cast<PDownfloor>(gen.genData)->dwflType = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp; 
                        std::static_pointer_cast<PDownfloor>(gen.genData)->isSeeSaw = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp; 
                        std::static_pointer_cast<PDownfloor>(gen.genData)->linkID = temp;
                        getline(genFile, line);

                        getline(genFile, line);

                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                    }
                    break;

                case PELT: {
                        // What the fuck.
                        std::cout << "Reading Pellet/Carry..." << std::endl;
                        gen.genData = std::make_shared<PPelt>();

                        std::string temp;
                        // Why?
                        genFile.seekg(-2, std::ios_base::seekdir::_S_cur);
                        genFile >> temp;
                        std::static_pointer_cast<PPelt>(gen.genData)->mgrID = stoi(temp);
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PPelt>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PPelt>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PPelt>(gen.genData)->rotation.z;
                        
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PPelt>(gen.genData)->localVersion;
                        getline(genFile, line);
                        

                        genFile >> temp;
                        std::static_pointer_cast<PPelt>(gen.genData)->peltID = stoi(temp);
                        getline(genFile, line);

                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                    }
                    break;

                case TEKI: {
                        std::cout << "Reading Teki" << std::endl;
                        
                        gen.genData = std::make_shared<PTeki>();
                        
                        std::string temp;
                        genFile >> temp; // skip {0005}
                        genFile >> temp;
                        std::cout << "ID " << temp << std::endl;
                        std::static_pointer_cast<PTeki>(gen.genData)->id = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp; // Get ID
                        

                        // Absolute dogshit.
                        std::string comment;
                        genFile >> comment >> comment;
                        if(gen.ver == VER3 || comment.find("teki_birth_type") != std::string::npos){
                            std::cout << "Reading Birth Type..." << std::endl;
                            std::static_pointer_cast<PTeki>(gen.genData)->birth_type = stoi(temp);
                            getline(genFile, line);
                            genFile >> temp;
                        }


                        std::static_pointer_cast<PTeki>(gen.genData)->num = stoi(temp);
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PTeki>(gen.genData)->direction;
                        getline(genFile, line);

                        genFile >> temp;
                        std::static_pointer_cast<PTeki>(gen.genData)->isMultispawn = stoi(temp);
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PTeki>(gen.genData)->spawnRadius;
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PTeki>(gen.genData)->scale;
                        getline(genFile, line);

                        genFile >> temp;
                        std::static_pointer_cast<PTeki>(gen.genData)->treasureID = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp;
                        std::static_pointer_cast<PTeki>(gen.genData)->pelletColor = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp;
                        std::static_pointer_cast<PTeki>(gen.genData)->pelletValue = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp;
                        std::static_pointer_cast<PTeki>(gen.genData)->pelletMin = stoi(temp);
                        getline(genFile, line);

                        genFile >> temp;
                        std::static_pointer_cast<PTeki>(gen.genData)->pelletMax = stoi(temp);
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PTeki>(gen.genData)->dropPelletChance;
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PTeki>(gen.genData)->versionID;
                        getline(genFile, line);

                        if(std::static_pointer_cast<PTeki>(gen.genData)->versionID == "{0000}"){
                            genFile >> std::static_pointer_cast<PTeki>(gen.genData)->fly;
                            getline(genFile, line);

                            genFile >> std::static_pointer_cast<PTeki>(gen.genData)->slide;
                            getline(genFile, line);
                        }

                        // Skip suffix
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);

                    }
                    break;

                case PIKI: {
                        std::string tempID;

                        std::cout << "Reading Wild Pikmin..." << std::endl;
                        gen.genData = std::make_shared<PPiki>();
                    
                        genFile >> tempID >> tempID;
                        std::cout << tempID << std::endl;
                        std::static_pointer_cast<PPiki>(gen.genData)->pikiColor = std::stoi(tempID);
                        getline(genFile, line);

                        genFile >> tempID >> tempID >> tempID;
                        std::cout << tempID << std::endl;
                        std::static_pointer_cast<PPiki>(gen.genData)->pikiNumber = std::stoi(tempID);
                        getline(genFile, line);

                        genFile >> tempID >> tempID >> tempID;
                        std::cout << tempID << std::endl;
                        std::static_pointer_cast<PPiki>(gen.genData)->pikiWild = std::stoi(tempID);
                        getline(genFile, line);

                        getline(genFile, line); // {_eof}
                        getline(genFile, line); // }
                        getline(genFile, line); // }
                        
                    }
                    break;

                case CAVE: {
                        getline(genFile, line);
                        std::cout << "Reading Cave..." << std::endl;
                        gen.genData = std::make_shared<PCave>();
                        genFile >> std::static_pointer_cast<PCave>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PCave>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PCave>(gen.genData)->rotation.z;
                    
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PCave>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        getline(genFile, std::static_pointer_cast<PCave>(gen.genData)->caveInfoFilename);
                        getline(genFile, std::static_pointer_cast<PCave>(gen.genData)->stageID);
                        
                        getline(genFile, line); // # Fog Parm
                        getline(genFile, line); // # {

                        std::string fogParam;
                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.startZ;
                        getline(genFile, line);
                        
                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.endZ;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.startTime;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.endTime;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.color.r;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.color.g;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.color.b;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.distance;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.enterDist;
                        getline(genFile, line);

                        genFile >> fogParam >> fogParam >>  std::static_pointer_cast<PCave>(gen.genData)->fog.exitDist;
                        getline(genFile, line);

                        getline(genFile, line); // {_eof}

                        getline(genFile, line); // # }

                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);                        
                    }
                    break;

                case BARL: {
                        getline(genFile, line);
                        std::cout << "Reading Barrel..." << std::endl;
                        gen.genData = std::make_shared<PBarrel>();
                        genFile >> std::static_pointer_cast<PBarrel>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PBarrel>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PBarrel>(gen.genData)->rotation.z;
                    
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PBarrel>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);                        
                    }
                    break;

                case BRDG: {
                        getline(genFile, line);
                        std::cout << "Reading Bridge..." << std::endl;
                        gen.genData = std::make_shared<PBridge>();
                        genFile >> std::static_pointer_cast<PBridge>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PBridge>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PBridge>(gen.genData)->rotation.z;
                    
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        std::string temp;
                        genFile >> temp;
                        std::static_pointer_cast<PBridge>(gen.genData)->type = std::stoi(temp);
                        getline(genFile, line);
                        
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);                        
                    }
                    break;

                case DGAT: {
                        getline(genFile, line);
                        std::cout << "Reading Electric Gate..." << std::endl;
                        gen.genData = std::make_shared<PGate>();
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->rotation.z;
                    
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->hp;
                        getline(genFile, line);
                        
                        std::static_pointer_cast<PGate>(gen.genData)->color = 3;
                        
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);                        
                    }
                    break;

                case GATE: {
                        getline(genFile, line);
                        std::cout << "Reading Gate..." << std::endl;
                        gen.genData = std::make_shared<PGate>();
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->rotation.x;
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->rotation.y;
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->rotation.z;
                    
                        getline(genFile, line);

                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->localVersion;
                        getline(genFile, line);
                        
                        genFile >> std::static_pointer_cast<PGate>(gen.genData)->hp;
                        getline(genFile, line);
                        
                        std::string temp;
                        genFile >> temp;
                        std::static_pointer_cast<PGate>(gen.genData)->color = stoi(temp);
                        getline(genFile, line);

                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);
                        getline(genFile, line);                        
                    }
                    break;
                default: {
                        // This doesn't always work properly. Boo. 
                        std::stack<int> brackets;
                        
                        getline(genFile, line);
                        brackets.push(1);

                        while(brackets.size() != 0){
                            getline(genFile, line);
                            
                            if(line.find("{") != std::string::npos && line.find("}") != std::string::npos){
                                continue; 
                            }

                            std::cout << "Bracket stack size " << brackets.size() << " line: " << line << std::endl;
                        
                            if(line.find("{") != std::string::npos) brackets.push(1);
                            if(line.find("}") != std::string::npos) brackets.pop();
                        }

                        getline(genFile, line);
                        
                        if(line.find("gen base") != std::string::npos){ // skip past genbase if we run into it
                            getline(genFile, line);
                            getline(genFile, line);
                            getline(genFile, line);
                            getline(genFile, line);
                        }
                        
                    }
                    break;
                }

                

                gens.generators.push_back(std::make_shared<PGenerator>(gen));

            }

            genTxt.close();
        }

        return gens;
    }

    // This one is fine, actually.

    PRoute ParseRouteFile(std::filesystem::path routePath){
        PRoute routes;
        std::ifstream routeTxt(routePath);

        if(routeTxt){
            
            std::string routeTxtAsStr((std::istreambuf_iterator<char>(routeTxt)), (std::istreambuf_iterator<char>()));
            //genTxtAsStr = LGenUtility::SjisToUtf8(genTxtAsStr);
            
            std::stringstream routeFile(routeTxtAsStr);

            std::string temp, line;

            routeFile >> temp;
            int waypointCount = std::stoi(temp);
            getline(routeFile, line); // Consume the rest of this line

            routes.waypoints = std::vector<std::shared_ptr<PWaypoint>>(waypointCount);
            for (size_t i = 0; i < waypointCount; i++){
                routes.waypoints[i] = std::make_shared<PWaypoint>();
            }
            

            for(int wp = 0; wp < waypointCount; wp++){
            
                getline(routeFile, line);

                getline(routeFile, line); // {

                routeFile >> temp;
                int index = std::stoi(temp);
                getline(routeFile, line); // # index

                std::shared_ptr<PWaypoint> waypoint = routes.waypoints[index];
                
                routeFile >> temp;
                int numLinks = std::stoi(temp);
                getline(routeFile, line); // # numLinks

                for(int linkIdx = 0; linkIdx < numLinks; linkIdx++){
                    routeFile >> temp;
                    int linkedWaypoint = std::stoi(temp);

                    waypoint->links.push_back(routes.waypoints[linkedWaypoint]);

                    getline(routeFile, line); // # link idx
                }

                routeFile >> waypoint->position.x;
                routeFile >> waypoint->position.y;
                routeFile >> waypoint->position.z;
                getline(routeFile, line); // read position

                getline(routeFile, line); // }
            }

        }

        return routes;
    }
};
