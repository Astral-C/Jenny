#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <array>
#include <map>
#include <format>
#include <glm/glm.hpp>
#include <J3D/Data/J3DModelInstance.hpp>

const std::string EntityTypeNames[] = {
    "Onion",
    "Piki",
    "Cave",
    "Teki",
    "Pellet",
    "Bridge",
    "Gate",
    "dgat?",
    "Rock",
    "Barrel",
    "Plant",
    "ujms?",
    "Weed :)",
    "Downfloor",
    "pkhd?",
    "mitu?",
    "Hole",
    "Warp"
};

const std::string TreasureInternalName[] = {
    "ahiru",
    "apple",
    "apple_blue",
    "bane",
    "baum_kuchen",
    "be_dama_red",
    "be_dama_yellow",
    "be_dama_blue",
    "bell",
    "bey_goma",
    "bird_hane",
    "bolt",
    "channel",
    "chess_king_black",
    "chess_king_white",
    "chess_queen_black",
    "chess_queen_white",
    "chocolate",
    "chocoichigo",
    "chocowhite",
    "compact",
    "cookie",
    "creap",
    "diamond_red",
    "diamond_blue",
    "diamond_green",
    "doll",
    "donguri",
    "donutschoco",
    "donutsichigo",
    "donutswhite",
    "flower_red",
    "flower_blue",
    "toy_gentle",
    "toy_lady",
    "toy_dog",
    "toy_cat",
    "gear",
    "gold_medal",
    "gum_tape",
    "halloween",
    "haniwa",
    "ichigo",
    "juji_key",
    "kan",
    "kinoko",
    "kouseki_suisyou",
    "kumakibori",
    "locket",
    "makigai",
    "matu_bokkuri",
    "milk_cap",
    "momiji_normal",
    "momiji_kare",
    "momiji_red",
    "nut",
    "tatebue",
    "saru_head",
    "sensya",
    "sinkukan",
    "silver_medal",
    "tel_dial",
    "toy_ring_a_red",
    "toy_ring_a_blue",
    "toy_ring_a_green",
    "toy_ring_b_red",
    "toy_ring_b_blue",
    "toy_ring_b_green",
    "toy_ring_c_red",
    "toy_ring_c_green",
    "toy_ring_c_blue",
    "toy_teala",
    "turi_uki",
    "watch",
    "Xmas_item",
    "yoyo_red",
    "yoyo_yellow",
    "yoyo_blue",
    "flask",
    "elec",
    "fire",
    "gas",
    "water",
    "mojiban",
    "futa_a_gold",
    "futa_a_silver",
    "kan_b_gold",
    "kan_b_silver",
    "ahiru_head",
    "loozy",
    "teala_dia_a",
    "teala_dia_b",
    "teala_dia_c",
    "teala_dia_d",
    "dia_a_red",
    "dia_a_blue",
    "dia_a_green",
    "dia_b_red",
    "dia_b_blue",
    "dia_b_green",
    "dia_c_red",
    "dia_c_green",
    "dia_c_blue",
    "kinoko_doku",
    "kinoko_tubu",
    "sinkukan_b",
    "sinkukan_c",
    "bolt_l",
    "gum_tape_s",
    "baum_kuchen_s",
    "ichigo_l",
    "cookie_m_l",
    "cookie_u",
    "cookie_u_l",
    "cookie_s",
    "cookie_s_l",
    "donguri_l",
    "fire_helmet",
    "nut_l",
    "be_dama_red_l",
    "be_dama_yellow_l",
    "be_dama_blue_l",
    "diamond_red_l",
    "diamond_blue_l",
    "diamond_green_l",
    "bell_red",
    "bell_blue",
    "bell_yellow",
    "bane_red",
    "bane_blue",
    "bane_yellow",
    "juji_key_fc",
    "tape_red",
    "tape_yellow",
    "tape_blue",
    "leaf_normal",
    "leaf_yellow",
    "leaf_kare",
    "denchi_3_red",
    "denchi_3_black",
    "denchi_2_red",
    "denchi_2_black",
    "denchi_1_red",
    "denchi_1_black",
    "castanets",
    "otama",
    "robot_head",
    "j_block_red",
    "j_block_yellow",
    "j_block_green",
    "j_block_blue",
    "j_block_white",
    "akagai",
    "hotate",
    "sinjyu",
    "donutschoco_s",
    "donutsichigo_s",
    "donutswhite_s",
    "gear_silver",
    "compact_make",
    "chocolate_l",
    "chocoichigo_l",
    "chocowhite_l",
    "g_futa_kyodo",
    "g_futa_titiyas",
    "g_futa_kyusyu",
    "g_futa_sikoku",
    "g_futa_kajiwara",
    "g_futa_koiwai",
    "g_futa_hirosima",
    "g_futa_kyosin",
    "g_futa_sakotani",
    "g_futa_daisen",
    "g_futa_hiruzen",
    "g_futa_kitaama",
    "g_futa_nihonraku",
    "kan_maruha",
    "kan_nichiro",
    "kan_iwate",
    "kan_kyokuyo",
    "kan_meidiya",
    "kan_imuraya",
    "wadou_kaichin",
    "kuri",
    "uji_jisyaku",
    "badminton",
    "medama_yaki",
    "whistle"
};

const std::string EnemyInternalName[] = {
    "Pelplant",
    "Kochappy",
    "Chappy",
    "BluePom",
    "RedPom",
    "YellowPom",
    "BlackPom",
    "WhitePom",
    "RandPom",
    "Kogane",
    "Wealthy",
    "Fart",
    "UjiA",
    "UjiB",
    "Tobi",
    "Armor",
    "Qurione",
    "Frog",
    "MaroFrog",
    "Rock",
    "Hiba",
    "GasHiba",
    "ElecHiba",
    "Sarai",
    "Tank",
    "Wtank",
    "Catfish",
    "Tadpole",
    "ElecBug",
    "Mar",
    "Queen",
    "Baby",
    "Demon",
    "FireChappy",
    "SnakeCrow",
    "KumaChappy",
    "Bomb",
    "Egg",
    "PanModoki",
    "Huefukimushi",
    "OoPanModoki",
    "Fuefuki",
    "BlueChappy",
    "YellowChappy",
    "BlueKochappy",
    "YellowKochappy",
    "Tanpopo",
    "Clover",
    "HikariKinoko",
    "Ooinu_s",
    "Ooinu_l",
    "Wakame_s",
    "Wakame_l",
    "KingChappy",
    "Miulin",
    "Hanachirashi",
    "Damagumo",
    "Kurage",
    "BombSarai",
    "FireOtakara",
    "WaterOtakara",
    "GasOtakara",
    "ElecOtakara",
    "Jigumo",
    "UNUSED",
    "Imomushi",
    "Houdai",
    "LeafChappy",
    "TamagoMushi",
    "BigFoot",
    "SnakeWhole",
    "UmiMushi",
    "OniKurage",
    "BigTreasure",
    "Stone",
    "Kabuto",
    "KumaKochappy",
    "ShijimiChou",
    "MiniHoudai",
    "Sokkuri",
    "Tukushi",
    "Watage",
    "Pom",
    "PanHouse",
    "Hana",
    "DaiodoRed",
    "DaiodoGreen",
    "Magaret",
    "Nekojarashi",
    "Chiyogami",
    "Zenmai",
    "KareOoinu_s",
    "KareOoinu_l",
    "BombOtakara",
    "DangoMushi",
    "Rkabuto",
    "Fkabuto",
    "FminiHoudai",
    "Tyre",
    "BlackMan",
    "UmiMushiBase",
    "UmiMushiBlind"
};

typedef enum {
    VER1,   // = "{v0.1}"",
    VER3    // = "{v0.3}"
} PGenVersion;

typedef enum {
    ONYN,
    PIKI,
    CAVE,
    TEKI,
    PELT,
    BRDG,
    GATE,
    DGAT,
    ROCK,
    BARL,
    PLNT,
    UJMS,
    WEED,
    DWFL,
    PKHD,
    MITU,
    HOLE,
    WARP,
    TAG_MAX
} PObjectTag;

const std::map<std::string, PObjectTag> PObjectTagStr = {
    {"{onyn}", ONYN}, {"{piki}", PIKI}, {"{cave}", CAVE}, {"{teki}", TEKI}, {"{pelt}", PELT}, {"{brdg}", BRDG},
    {"{gate}", GATE}, {"{dgat}", DGAT}, {"{rock}", ROCK}, {"{barl}", BARL}, {"{plnt}", PLNT}, {"{ujms}", UJMS}, 
    {"{weed}", WEED}, {"{dwfl}", DWFL}, {"{pkhd}", PKHD}, {"{mitu}", MITU}, {"{hole}", HOLE}, {"{warp}", WARP},
    {"", TAG_MAX}
};

const std::map<std::string, PGenVersion> PGenVersionStr = {{"{v0.1}", VER1}, {"{v0.3}", VER3}};

struct PEntity {
    std::shared_ptr<J3DModelInstance> mModel { nullptr };
    virtual std::string GetName(PObjectTag type){ return EntityTypeNames[type]; }
};

struct POnion : PEntity {
    glm::vec3 rotation;
    std::string localVersion;
    uint8_t index;
    uint8_t afterBoot;

    std::string GetName(PObjectTag type){ return (index == 4 ? "Ship" : "Onion"); }

    POnion(){}
    ~POnion(){}
};

struct PDownfloor : PEntity {
    glm::vec3 rotation;
    std::string localVersion;
    
    uint8_t pressWeight;
    uint8_t dwflType;
    uint8_t isSeeSaw;
    std::string linkID;

    std::string GetName(PObjectTag type){ return (dwflType == 0 ? "Small Block DWFL" : (dwflType == 1 ? "Large Block DWFL" : "Paper Bag DWFL")); }

    PDownfloor(){}
    ~PDownfloor(){}
};

struct PWeed : PEntity {
    glm::vec3 rotation;
    std::string localVersion;
    
    uint8_t weedAmount;
    uint8_t type;

    std::string GetName(PObjectTag type){ return (type == 1 ? "Nectar Grass" : "Nectar Rocks"); }

    PWeed(){}
    ~PWeed(){}
};

struct PPiki : PEntity {
    uint8_t pikiColor;
    uint8_t pikiNumber;
    uint8_t pikiWild; //?

    std::string GetName(PObjectTag type){ return std::format("{0} {1} Pikmin", pikiNumber, (pikiColor == 0 ? "Blue" : (pikiColor == 1 ? "Red" : (pikiColor == 2 ? "Yellow" : (pikiColor == 3 ? "Purple" : "White"))))); }

    PPiki(){}
    ~PPiki(){}
};


struct PGate : PEntity {
    glm::vec3 rotation;
    std::string localVersion;
    
    float hp;
    uint8_t color;

    std::string GetName(PObjectTag type){ return (color == 1 ? "Light Gate" : (color == 3 ? "Electric Gate" : "Dark Gate")); }

    PGate(){}
    ~PGate(){}
};

struct PBarrel : PEntity {
    glm::vec3 rotation;
    std::string localVersion;

    std::string GetName(PObjectTag type){ return "Drain Clog"; }

    PBarrel(){}
    ~PBarrel(){}
};

struct FogParams {
    float startZ;
    float endZ;
    float startTime;
    float endTime;

    glm::vec3 color;
    
    float distance;
    float enterDist;
    float exitDist;
    
};

struct PCave : PEntity {
    glm::vec3 rotation;
    std::string localVersion;

    std::string caveInfoFilename; //units.txt can be skipped
    std::string stageID;

    FogParams fog;

    std::string GetName(PObjectTag type){ return "Cave"; }

    PCave(){}
    ~PCave(){}
};

struct PPelt : PEntity {
    glm::vec3 rotation;
    std::string localVersion;

    uint8_t mgrID;
    uint8_t peltID;

    std::string GetName(PObjectTag type){ return TreasureInternalName[peltID]; }

    PPelt(){}
    ~PPelt(){}
};

struct PBridge : PEntity {
    glm::vec3 rotation;
    std::string localVersion;
    
    int type;

    std::string GetName(PObjectTag type){ return (type == 0 ? "Short Bridge" : (type == 2 ? "Long Bridge" : "Sloped Bridge")); }

    PBridge(){}
    ~PBridge(){}
};

struct PTeki : PEntity {
    uint8_t id;

    uint8_t birth_type;

    uint8_t num;

    float direction;

    uint8_t isMultispawn;

    float spawnRadius;
    float scale;

    uint16_t treasureID;

    uint8_t pelletColor;
    uint8_t pelletValue;
    uint8_t pelletMin;
    uint8_t pelletMax;

    float dropPelletChance;

    uint8_t pelletType; // pellet posy only!

    uint8_t growthAmount;

    std::string versionID;

    float fly;
    float slide;

    std::string GetName(PObjectTag type){ return EnemyInternalName[id]; }

    PTeki(){}
    ~PTeki(){}
};

struct PGenerator {
    PGenVersion ver;         // Generator version
    int reserved;            // Affects loading behavior, 0 = Loads once, (1, ..) = Can load multiple times, 
                             // (.., -1) = An affront to GOD that is somehow used
    uint32_t respawnDays;        // 0 = always spawn, even after leaving cave, (1, ..) = Wait this many days to respawn
    // ubyte[] comment; #NOT NECESSARY FOR OBJECT OPERATION
    glm::vec3 pos;                // Spawn position of object
    glm::vec3 offset;             // Position offset from spawn position
    PObjectTag entityType;  // INTERNAL: Used at runtime to know what the stored object is
    std::shared_ptr<PEntity> genData;    // Contains the specific object information
    std::string GetName() { if(genData != nullptr){ return genData->GetName(entityType); } else { return EntityTypeNames[entityType]; } }
};

struct PGenCollection {
    PGenVersion ver;       // The file version
    glm::vec3 startPos;         // XYZ starting position of generator
    float startDir;        // Starting angle of generator
    uint32_t numGen;           // Number of objects contained
    std::vector<std::shared_ptr<PGenerator>> generators; // Generator Object array
};

struct PWaypoint {
    glm::vec3 position;
    std::vector<std::weak_ptr<PWaypoint>> links;
};

struct PRoute {
    std::vector<std::shared_ptr<PWaypoint>> waypoints;
};

namespace PGenParser {
    PGenCollection ParseGenFile(std::filesystem::path genPath);
    PRoute ParseRouteFile(std::filesystem::path routePath);
};
