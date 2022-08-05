#ifndef ToolTipComponent_h
#define ToolTipComponent_h

#include <Components/ComponentIds.h>

namespace Ice {

namespace GUI {
	class Widget;
}
struct CameraComponent;

struct ToolTipComponent {
	static constexpr EntityComponentId id() noexcept { return TOOLTIP_COMPONENT; }

	GUI::Widget* m_pToolTip{ nullptr };
	std::function<const CameraComponent& ()> m_getCamFunc;
};

}

#endif