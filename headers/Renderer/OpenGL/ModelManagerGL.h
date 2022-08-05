#pragma once

#include <unordered_map>
#include <Interfaces/IModelManager.h>
#include <Renderer/OpenGL/RenderObjectGL.h>

namespace Ice {

struct Model;
struct MeshComponent;
class IShaderProgram;

class ModelManagerGL : public IModelManager {

    std::unordered_map<MeshComponent*, std::unique_ptr<RenderObjectGL>> m_mModels;
	std::unordered_map<std::string, std::unique_ptr<IShaderProgram>> m_mShaderPrograms;
	std::unordered_map<std::string, std::unique_ptr<TextureGL>> m_mTextures;

public:
	void registerModel(const Model* pModel) noexcept override;
    void unregisterModel(const Model* pModel) noexcept override;
	RenderObjectGL* getModel(MeshComponent*);
};

}