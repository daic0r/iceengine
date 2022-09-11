//
//  Skybox.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Skybox_h
#define Skybox_h

#include <Components/MeshComponent.h>
#include <array>

namespace Ice {

struct Skybox {
    friend class SkyboxRendererGL;
    
    // in NDC coords (Z pointing forward, ranging from -1 to 1 in all dimensions)
    static constexpr std::array<glm::vec3, 4> Front {
        glm::vec3{ -1, -1, 1 },
        glm::vec3{ 1, -1, 1 },
        glm::vec3{ 1, 1, 1 },
        glm::vec3{ -1, 1, 1 }
    };
    
    static constexpr std::array<glm::vec3, 4> Back {
        glm::vec3{ 1, -1, -1 },
        glm::vec3{ -1, -1, -1 },
        glm::vec3{ -1, 1, -1 },
        glm::vec3{ 1, 1, -1 }
    };
    
    static constexpr std::array<glm::vec3, 4> Left {
        glm::vec3{ -1, -1, -1 },
        glm::vec3{ -1, -1, 1 },
        glm::vec3{ -1, 1, 1 },
        glm::vec3{ -1, 1, -1 }
    };
        
    static constexpr std::array<glm::vec3, 4> Right {
        glm::vec3{ 1, -1, 1 },
        glm::vec3{ 1, -1, -1 },
        glm::vec3{ 1, 1, -1 },
        glm::vec3{ 1, 1, 1 }
    };
    
    static constexpr std::array<glm::vec3, 4> Top {
        glm::vec3{ -1, 1, 1 },
        glm::vec3{ 1, 1, 1 },
        glm::vec3{ 1, 1, -1 },
        glm::vec3{ -1, 1, -1 }
    };
    
    static constexpr std::array<glm::vec3, 4> Bottom {
        glm::vec3{ -1, -1, -1 },
        glm::vec3{ 1, -1, -1 },
        glm::vec3{ 1, -1, 1 },
        glm::vec3{ -1, -1, 1 }
    };
    
    static constexpr std::array<unsigned int, 6> Indices = {
        0, 1, 2, 2, 3, 0
    };
    
public:
    
    Skybox();
    ~Skybox() = default;
//    void loadCubeMap();
    
private:
    MeshComponent m_mesh;
//    std::unique_ptr<CubeTexture> m_pCubeTexture;
};

}

#endif /* Skybox_h */
