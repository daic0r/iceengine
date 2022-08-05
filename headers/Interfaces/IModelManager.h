#pragma once

namespace Ice {

struct Model;
class RenderObjectGL;
struct MeshComponent;

class IModelManager {
public:
	virtual void registerModel(const Model*) noexcept = 0;
	virtual void unregisterModel(const Model*) noexcept = 0;
	virtual RenderObjectGL* getModel(MeshComponent*) = 0;
	virtual ~IModelManager() = default;
};

}