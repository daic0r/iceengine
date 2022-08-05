//
//  ModelImporterCollada.h
//  IceCraft
//
//  Created by Matthias Gruen on 12.10.20.
//  Copyright � 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelImporterCollada_h
#define ModelImporterCollada_h

#include <IceEngine.h>
#include <string>
#include <map>
#include <vector>
#include <System/RenderMaterial.h>
#include <glm/mat4x4.hpp>
#include <ModelAnimation/Joint.h>

namespace rapidxml {
    template<typename Ch> class xml_node;
}

namespace Ice {

struct MeshData;
class RenderMaterial;
struct SkeletonComponent;
struct AnimatedMeshComponent;
struct ModelAnimationComponent;
struct MeshComponent;

class ICEENGINE_API ModelImporterCollada {
    
    struct sJoint;
    
    glm::mat4 m_correctiveTransform;    // Transform to bring the coordinates into the right system, since in Blender Z is up
    glm::mat4 m_invCorrectiveTransform;    // Transform to bring the coordinates into the right system, since in Blender Z is up

    std::string m_strFile;
    std::map<std::string, RenderMaterial> m_mMaterials;
    // Vertex at index n is affected by the joints in the map <joint ID, weight>
    std::vector<std::map<float, size_t>> m_vVertexWeights;
    std::map<std::string, size_t> m_msId2Joint; // sid found in joint name array under library_controllers, referenced in library_visual_scenes
    std::map<std::string, size_t> m_mId2Joint; // id found in node tages under library_visual_scenes, referenced in animations in the channel tag
    std::vector<sJoint> m_vJoints;  // Joints in ordered by their index
    glm::mat4 m_bindShapeMatrix;    // From bind_shape_matrix tag
    glm::mat4 m_invBindShapeMatrix;  // from top level node under visual_scene
    size_t m_nRootJoint;
    std::map<std::string, std::string> m_mImages;   // Images used by textures

    // Collada accessor descriptor
    struct sAccessor {
        int nCount{ -1 };
        int nStride{ -1 };
        std::map<std::string, std::string> mParams;
    };

    // Source descriptor
    struct sSource {
        std::vector<float> vBuffer;
        std::vector<std::string> vStringBuffer;
        sAccessor accessor;
    };
    
    struct sJoint {
        std::string strName;
        int nId{ -1 };
        glm::mat4 invBindTransform;
        glm::mat4 matrix;
        int nParentIdx{ -1 };
        std::vector<size_t> vChildren;
        std::map<float, glm::mat4> mKeyframes;
    };

    static std::string stripLeadingChar(const std::string&);
    static glm::mat4 getMatrixFromNode(rapidxml::xml_node<char>*);
    static glm::mat4 getTransformFromNode(rapidxml::xml_node<char>*);
    
    void loadMaterials(rapidxml::xml_node<char>*);
    void loadJointData(rapidxml::xml_node<char>*);
    void loadJointHierarchy(rapidxml::xml_node<char>*);
    void traverseJointHierarchy(rapidxml::xml_node<char>*);
    void loadAnimations(rapidxml::xml_node<char>*);
    void loadImages(rapidxml::xml_node<char>*);

    static std::map<std::string, sSource> loadSources(rapidxml::xml_node<char>*);

    static void linkUpInputs(rapidxml::xml_node<char>* pParentNode, const std::map<std::string, sSource>& mSrcBuffers, std::map<int, std::string>& mOutOffset2Semantic, std::map<int, const sSource*>& mOutInputs);
    
    void sJointToJoint(const sJoint&, Joint&) const noexcept;

public:

    ModelImporterCollada(const std::string&);

    const auto& materials() const noexcept { return m_mMaterials; }
    const auto& vertexWeights() const noexcept { return m_vVertexWeights; }
    const auto& joints() const noexcept { return m_vJoints; }
    const auto& invBindShapeMatrix() const noexcept { return m_invBindShapeMatrix; }
    
    bool import(std::map<std::string, MeshComponent>&, std::map<std::string, AnimatedMeshComponent>&) noexcept;
    
    void toSkeletonComponent(SkeletonComponent&) const noexcept;
    void toModelAnimationComponent(ModelAnimationComponent&) const noexcept;
    
};

}

#endif /* ModelImporterCollada_h */
