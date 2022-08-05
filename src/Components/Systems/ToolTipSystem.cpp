#include <Components/Systems/ToolTipSystem.h>
#include <Components/InputReceiverComponent.h>
#include <Components/MeshComponent.h>
#include <Components/CameraComponent.h>
#include <Components/TransformComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <System/Extents.h>
#include <GUI/MousePicker.h>
#include <System/AABB.h>
#include <System/Ray.h>
#include <System/SystemServices.h>
#include <GUI/WidgetManager.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <Utils/ScopedTimeMeasurement.h>

namespace Ice {

void ToolTipSystem::addToolTip(Entity e, GUI::Widget* w, const std::function<const CameraComponent& ()>& getCamFunc) noexcept {

   auto mouseHandler = [w, &getCamFunc, ext = Extents3{}, bInitExt=false, this](Entity e, std::uint32_t event, SDL_MouseMotionEvent* pMotion, SDL_MouseButtonEvent* pButton, SDL_MouseWheelEvent* pWheel) mutable {
	   if (m_pMouseOverWidget != nullptr) {
		   return true;
	   }
        if (pWheel->type == SDL_MOUSEMOTION) {
            auto& bMouseOver = m_mMouseOver[w];
            if (!bInitExt) {
                const auto& modRef = entityManager.getComponent<ModelReferenceComponent>(e);
                const auto& meshComp = entityManager.getComponent<MeshComponent>(modRef.m_entity);
                ext = meshComp.extents();
                bMouseOver = false;
                bInitExt = true;
            }
            const auto& transComp = entityManager.getSharedComponentOr<TransformComponent>(e);
            const auto& camComp = getCamFunc();
            glm::vec4 min4{ ext.minPoint, 1.0f };
            glm::vec4 max4{ ext.maxPoint, 1.0f };
            min4 = transComp.m_transform * min4;
            max4 = transComp.m_transform * max4;
            AABB bb{ min4, max4 };
            MousePicker pick{ camComp.m_camera.matrix() };
            Ray r{ camComp.m_camera.position(), pick.getMouseRay() };
            const bool bIntersects = bb.intersects(r);
            if (bIntersects && !bMouseOver) {
                bMouseOver = true;
                w->setVisible(true);
				m_pMouseOverWidget = w;
            }
            else if (!bIntersects && bMouseOver) {
                bMouseOver = false;
                w->setVisible(false);
            }
            else if (bIntersects && w) {
                w->setConstraintsDirty(true, true);
            }
        }
		++m_nCheck;
        return true;
    };

    if (!entityManager.hasComponent<InputReceiverComponent>(e))
        entityManager.addComponent(e, InputReceiverComponent{});

    entityManager.getComponent<InputReceiverComponent>(e, [&mouseHandler](InputReceiverComponent& inp) {
        inp.m_vMouseHandlers.push_back(mouseHandler);
    });
}

void ToolTipSystem::onEntityAdded(Entity e) noexcept
{
    auto& comp = entityManager.getComponent<ToolTipComponent>(e, [this, e](ToolTipComponent& comp) {
        comp.m_pToolTip->setPointConstraint(Ice::GUI::HorizontalAlignment::LEFT, std::make_unique<Ice::GUI::DynamicPointConstraint>([](GUI::Widget* w) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            return systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(x + 5) - 1.0f;
            }));
        comp.m_pToolTip->setPointConstraint(Ice::GUI::VerticalAlignment::TOP, std::make_unique<Ice::GUI::DynamicPointConstraint>([](GUI::Widget* w) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            return 1.0f - systemServices.getGraphicsSystem()->getNormalizedPixelsVertical(y + 5);
            }));

        addToolTip(e, comp.m_pToolTip, comp.m_getCamFunc);
    });
}

bool ToolTipSystem::update(float) noexcept {
	m_pMouseOverWidget = nullptr;
	m_nCheck = 0;
	return true;
}


}
