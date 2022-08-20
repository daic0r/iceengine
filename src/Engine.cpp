//
//  Engine.cpp
//  CovidSimulator
//
//  Created by Matthias Gruen on 04.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Engine.h>
#include <SDL2/SDL.h>
//#include <System/Controller.h>
#include <System/Scene.h>
#include <Interfaces/IMasterRenderer.h>
#include <System/SystemServices.h>
#include <Interfaces/ILoader.h>
#include <World/WaterTile.h>
#include <iostream>
#include <vector>
#include <System/Camera.h>
#include <World/Terrain.h>
#include <System/ModelInstance.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Interfaces/IGame.h>
#include <Events/EventQueue.h>
#include <Entities/Entity.h>
#include <Entities/EntityManager.h>
#include <Entities/EntityComponentSystem.h>
#include <System/Config.h>
#include <Components/Systems/ModelManagerSystem.h>
#include <Components/Systems/CameraControllerSystem.h>
#include <Components/Systems/ModelManagerSystem.h>
#include <Components/Systems/EventHandlingSystem.h>
#include <Components/Systems/MasterRenderingSystem.h>
#include <Interfaces/IGraphicsSystem.h>
#include <Components/Systems/AnimatedModelManagerSystem.h>
#include <Components/Systems/ModelAnimationSystem.h>
#include <Components/Systems/PathAnimationSystem.h>
#include <Components/Systems/RoadNetworkSystem.h>
#include <Components/Systems/SaveSystem.h>
#include <Components/Systems/ToolTipSystem.h>
#include <Components/Systems/MeshGroupSystem.h>
#include <Components/Systems/InfectionSystem.h>
#include <Components/Systems/DayNightCycleSystem.h>
#include <Components/Systems/TerrainSystem.h>
#include <chrono>
#include <GUI/WidgetManager.h>
#include <System/ThreadPool.h>

//class Scene;

namespace Ice {

static std::unique_ptr<ModelManagerSystem> _pModelManager;
static std::unique_ptr<AnimatedModelManagerSystem> _pAnimatedModelManager;
static std::unique_ptr<EventHandlingSystem> _pEventSystem;
static std::unique_ptr<CameraControllerSystem> _pCamSystem;
static std::unique_ptr<MasterRenderingSystem> _pMasterRenderingSystem;
static std::unique_ptr<ModelAnimationSystem> _pModelAnimationSystem;
static std::unique_ptr<PathAnimationSystem> _pPathAnimationSystem;
static std::unique_ptr<RoadNetworkSystem> _pRoadNetworkSystem;
static std::unique_ptr<SaveSystem> _pSaveSystem;
static std::unique_ptr<ToolTipSystem> _pToolTipSystem;
static std::unique_ptr<MeshGroupSystem> _pMeshGroupSystem;
static std::unique_ptr<InfectionSystem> _pInfectionSystem;
static std::unique_ptr<DayNightCycleSystem> _pDayNightCycleSystem;
static std::unique_ptr<TerrainSystem> _pTerrainSystem;

bool Engine::init(const Config& config, ILoader *pLoader, std::unique_ptr<IGame> pGame) {
    systemServices.setConfig(config);
    
    m_pGame = std::move(pGame);
    
    systemServices.getGraphicsSystem()->init();
    _pMasterRenderingSystem = std::make_unique<MasterRenderingSystem>();
    _pModelManager = std::make_unique<ModelManagerSystem>();
    _pAnimatedModelManager = std::make_unique<AnimatedModelManagerSystem>();
    _pCamSystem = std::make_unique<CameraControllerSystem>();
    _pEventSystem = std::make_unique<EventHandlingSystem>();
    _pModelAnimationSystem = std::make_unique<ModelAnimationSystem>();
    _pPathAnimationSystem = std::make_unique<PathAnimationSystem>();
    _pRoadNetworkSystem = std::make_unique<RoadNetworkSystem>();
    _pSaveSystem = std::make_unique<SaveSystem>();
    _pToolTipSystem = std::make_unique<ToolTipSystem>();
    _pMeshGroupSystem = std::make_unique<MeshGroupSystem>();
	_pInfectionSystem = std::make_unique<InfectionSystem>();
	_pDayNightCycleSystem = std::make_unique<DayNightCycleSystem>();
    _pTerrainSystem = std::make_unique<TerrainSystem>();

    entityManager.registerComponentSystem<false>(_pModelManager.get());
    entityManager.registerComponentSystem<false>(_pAnimatedModelManager.get());
    entityManager.registerComponentSystem<false>(_pRoadNetworkSystem.get());
    entityManager.registerComponentSystem<false>(_pSaveSystem.get());
    entityManager.registerComponentSystem<false>(_pMasterRenderingSystem.get());
    entityManager.registerComponentSystem<false>(_pEventSystem.get());
    entityManager.registerComponentSystem<false>(_pMeshGroupSystem.get());
    entityManager.registerComponentSystem<false>(_pTerrainSystem.get());

    entityManager.registerComponentSystem<true>(_pToolTipSystem.get());
    entityManager.registerComponentSystem<true>(_pPathAnimationSystem.get());
    entityManager.registerComponentSystem<true>(_pModelAnimationSystem.get());
	entityManager.registerComponentSystem<true>(_pInfectionSystem.get());
    entityManager.registerComponentSystem<true>(_pCamSystem.get());
	entityManager.registerComponentSystem<true>(_pDayNightCycleSystem.get());

	entityManager.notifySystemsInitialized();

    auto bSuccess = true;
    if (pLoader != nullptr)
        bSuccess = pLoader->loadData();
    //ParticleManager::instance().init();
    
    m_pGame->init();
    
    return bSuccess;
}



bool Engine::run() {
    
    
    // Debug
    //1040, 100.034f, -910
//    WaterTile tile{ glm::vec2{ 1040, -890 }, Config::WATER_LEVEL, 80.0f };
    //Terrain terr{ 0, 0, "grassTerrain" };
    //Terrain terr2{ -1, 0, "grassTerrain" };
    //std::vector<WaterTile> vTiles { tile };
    
//    auto pWaterRenderer = dynamic_cast<WaterRendererGL*>(SystemServices::instance().getWaterRenderer());
//    WaterFramebuffersGL fbo;
    
//    std::string modelPath{ Config::BasePath };
//    modelPath.append("Assets/Lowpoly_tree_sample.obj");
//    ModelImporterOBJ mimp{ modelPath };
//    std::vector<float> vVerts, vTex, vNormals;
//    std::map<std::string, std::vector<unsigned int>> mIndices;
//    mimp.import(vVerts, vTex, vNormals, mIndices);
    
//    Model mod;
//    mod.setVertices(vVerts);
//    mod.setTexCoords(vTex);
//    mod.setNormals(vNormals);
//    //mod.setIndices(allIndices);
//    mod.setMaterials(mimp.materials());
//    mod.setMaterialIndices(mIndices);
//    mod.setTextureAtlasId("grassTerrain");
    //systemServices.getMasterRenderer()->registerModel(&mod);
    
//    auto coord_rand = std::bind(std::uniform_int_distribution<int>(0,2048),
//                               std::mt19937(time(0)));
    
//    float pos1x = 70.0f, pos1z = 100.0f;
//    float pos2x = 170.0f, pos2z = 200.0f;
//    float fHeight1, fHeight2;
//    terr.getHeight(pos1x, pos1z, fHeight1);
//    terr.getHeight(pos2x, pos2z, fHeight2);

//    std::vector<std::unique_ptr<ModelInstance>> vTrees;
//    for (int i = 0; i < 400; ++i) {
//        vTrees.emplace_back(std::make_unique<ModelInstance>(&mod));
//        float pos1x = coord_rand(), pos1z = coord_rand();
//        float fHeight1;
//        //terr.getHeight(pos1x, pos1z, fHeight1);
//        fHeight1 = 2.0f;
//        glm::mat4 worldTrans;
//        worldTrans = glm::translate(glm::mat4{1.0f}, glm::vec3{ pos1x, fHeight1, pos1z });
//        vTrees.back()->setWorldTransform(worldTrans);
//    }
    
//    ModelInstance inst{ &mod };
//    glm::mat4 worldTrans;
//    worldTrans = glm::translate(glm::mat4{1.0f}, glm::vec3{ pos1x, fHeight1, pos1z });
//    inst.setWorldTransform(worldTrans);
//
//    ModelInstance inst2{ &mod };
//    worldTrans = glm::translate(glm::mat4{1.0f}, glm::vec3{ pos2x, fHeight2, pos2z });
//    inst2.setWorldTransform(worldTrans);

    //Controller::instance().m_vTerrains.emplace_back(terr);

    // A basic main loop to prevent blocking
    bool is_running = true;
    m_nLastTicks = SDL_GetTicks();
    
    //std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();
    //int nFps{ 0 };
    while (is_running) {

        float nCurrentTicks = SDL_GetTicks();
        float fDeltaTime = (float)(nCurrentTicks - m_nLastTicks) / 1000.0f;
        m_nLastTicks = nCurrentTicks;
        //is_running = Controller::instance().handleEvents();
        //Controller::instance().player().interpolatePosition(fDeltaTime);
        //ParticleManager::instance().updateSystems(fDeltaTime);
        //ParticleManager::instance().update(fDeltaTime);
        //Controller::instance().camera().setPosition(Controller::instance().player().position());
        //world.sun().setPosition(Controller::instance().player().position() + glm::vec3{ 0, 100.0f, 0.0f });
       // Controller::instance().camera().setYawAndPitch(Controller::instance().player().yaw(), Controller::instance().player().pitch());

        //Scene* pScene = Controller::instance().getCurrentScene();
        //pScene->waterTiles().emplace_back(&tile);
        //pScene->terrains().emplace_back(&terr);
        //pScene->terrains().emplace_back(&terr2);
        //pScene->addInstance(&inst);
        //pScene->addInstance(&inst2);
//        for (const auto& tree : vTrees)
//            pScene->addInstance(tree.get());

        //systemServices.getEventQueue()->notify(true, true);
        //systemServices.getMasterRenderer()->render(fDeltaTime, &Controller::instance().camera(), pScene);


        systemServices.getEventQueue()->clearUserEvents();
        systemServices.getEventQueue()->clearInternalEvents();
    
        is_running =_pEventSystem->update(fDeltaTime);  // <-- Input events are generated here
        //plantSystem.update(fDeltaTime);
        is_running &= entityManager.update(fDeltaTime);
        m_pGame->update(fDeltaTime, systemServices.getEventQueue());
        _pMasterRenderingSystem->update(fDeltaTime);

        systemServices.getWidgetManager().update(fDeltaTime); // <-- Update widget manager AFTER the inputs have been processed so we can handle input here
        //SDL_GL_SwapWindow(MasterRendererGL::instance().window());
        
        /*
        ++nFps;
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime);
        if (dur.count() > 5000) {
            std::cout << std::dec << nFps / 5 << " fps\n";
            nFps = 0;
            lastTime = std::chrono::steady_clock::now();
        }
        */
    }
    
    return true;
}

}
