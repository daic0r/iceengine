//
//  AnimatedModelShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedModelShaderConfigurator_h
#define AnimatedModelShaderConfigurator_h

#include <ShadersGL/ModelShaderConfigurator.h>

namespace Ice {

class Joint;

class AnimatedModelShaderConfigurator : public ModelShaderConfigurator {
public:
    static constexpr size_t MAX_JOINTS = 50;
	static constexpr size_t MAX_INSTANCES = 5000;

private:
	GLint m_nTexObjUniformID{ -1 };
	GLuint m_nTexObjDataBuffer{ 0 };
	GLuint m_nTexObj{ 0 };
    
    void bindAttributes() const noexcept override;
    void getUniformLocations() noexcept override;
    
public:
	~AnimatedModelShaderConfigurator();
    void loadJointTransforms(const std::vector<glm::mat4>&) noexcept;
};

}


#endif /* AnimatedModelShaderConfigurator_h */
