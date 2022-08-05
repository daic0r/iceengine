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
    static constexpr std::array<float, 12> Front {
        -1, -1, 1,
        1, -1, 1,
        1, 1, 1,
        -1, 1, 1
    };
    
    static constexpr std::array<float, 12> Back {
        1, -1, -1,
        -1, -1, -1,
        -1, 1, -1,
        1, 1, -1
    };
    
    static constexpr std::array<float, 12> Left {
        -1, -1, -1,
        -1, -1, 1,
        -1, 1, 1,
        -1, 1, -1
    };
        
    static constexpr std::array<float, 12> Right {
        1, -1, 1,
        1, -1, -1,
        1, 1, -1,
        1, 1, 1
    };
    
    static constexpr std::array<float, 12> Top {
        -1, 1, 1,
        1, 1, 1,
        1, 1, -1,
        -1, 1, -1
    };
    
    static constexpr std::array<float, 12> Bottom {
        -1, -1, -1,
        1, -1, -1,
        1, -1, 1,
        -1, -1, 1
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
