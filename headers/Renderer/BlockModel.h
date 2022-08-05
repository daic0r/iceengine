//
//  BlockModel.h
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef BlockModel_h
#define BlockModel_h

#include "GLModel.h"
#include <glm/vec3.hpp>
#include <array>
#include "Types.h"

class Texture;

class BlockModel : public GLModel {
public:
    

    
    BlockModel();
    
    std::vector<Texture*> m_vFaceTexture;
    //virtual void render(GLuint uniformId);
    void setUniforms(const RenderEnvironment &) override {}
    
};

#endif /* BlockModel_h */
