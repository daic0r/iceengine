#ifndef ToolTipSystem_h
#define ToolTipSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/ToolTipComponent.h>
#include <unordered_map>

namespace Ice {

struct CameraComponent;
namespace GUI {
	class Widget;
}

class ToolTipSystem : public EntityComponentSystem<ToolTipComponent> {
	std::unordered_map<GUI::Widget*, bool> m_mMouseOver;
	GUI::Widget* m_pMouseOverWidget{ nullptr };
	int m_nCheck{ 0 };

	void addToolTip(Entity e, GUI::Widget* w, const std::function<const CameraComponent& ()>& getCamFunc) noexcept;
	void onEntityAdded(Entity) noexcept override;
	bool update(float) noexcept;

};

}

#endif