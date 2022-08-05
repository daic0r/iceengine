//
//  Controller.h
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include "Camera.h"
#include <thread>
#include <atomic>
#include "Player.h"
#include <vector>
#include <memory>
#include <World/Terrain.h>

namespace Ice {

struct ModelInstance;
class Scene;
class ILoader;

class Controller {
    friend class World;
    friend class Engine;
    
    Camera m_cam;
    int nLastX{}, nLastY{};
    bool firstMouse{ true };
    unsigned int m_lastTickCount{ 0 };
    unsigned int m_lastClickTick{ 0 };
    float m_fCamSpeed{ 10.0f }; // per 100 ms
    std::thread* m_pThread{ nullptr };
    //Player m_player;
    std::vector<std::unique_ptr<ModelInstance>> m_vModelInstances;
    std::unique_ptr<Scene> m_pLastScene;
    bool m_bClick{ false };
    std::vector<Terrain> m_vTerrains;
    
    Controller();
    ~Controller();
    void _updateCameraDirection(bool bClick);
    void startThread();
    
public:
    std::atomic_bool m_bDone { false };

    bool m_bHasGravityAnimation { false };

    static Controller& instance();
    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;
    
    inline auto& camera() noexcept { return m_cam; }
    //inline auto& player() noexcept { return m_player; }
    glm::mat4 viewMatrix() const noexcept;
    
    bool handleEvents();
    
    Scene* getCurrentScene();
    
    void addLoader(std::unique_ptr<ILoader>) noexcept;
};

}

#endif /* Controller_h */
