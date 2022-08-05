#include <Components/Systems/BuildingSystem.h>
#include <Entities/EntityManager.h>
#include <Components/TransformComponent.h>
#include <Components/MeshComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/Systems/RoadNetworkSystem.h>
#include <Components/PathSegmentComponent.h>
#include <Components/PathComponent.h>
#include <Components/LocationComponent.h>
#include <Components/CameraComponent.h>
#include <Components/ToolTipComponent.h>
#include <Components/SaveComponent.h>
#include <Components/BlueprintComponent.h>
#include <Components/GraphEdgeComponent.h>
#include <World/World.h>
#include <System/Tools.h>
#include <Entities/EntityComponentBuffer.h>
#include <GUI/WidgetManager.h>
#include <GUI/StackPanel.h>
#include <GUI/TextLabel.h>

namespace Ice {

Ice::Entity BuildingSystem::createInstance(Ice::Entity blueprintEnt, const glm::mat4& tr)
{
	using namespace Ice;

	auto retEnt = entityManager.createEntity();
    entityManager.addComponent(retEnt, TransformComponent{ tr });
    entityManager.addComponent(retEnt, ModelReferenceComponent{ blueprintEnt });
	auto buildingComp = entityManager.getComponent<BuildingComponent>(blueprintEnt);
	entityManager.addComponent(retEnt, buildingComp);
	entityManager.addComponent(retEnt, SaveComponent{});

	return retEnt;
}

void BuildingSystem::localizeBuilding(Ice::Entity e) noexcept {
	using namespace Ice;

	auto pRoadSystem = entityManager.getSystem<Ice::RoadNetworkSystem, false>();

	if (!entityManager.hasComponent<TransformComponent>(e))
		return;

	if (entityManager.hasComponent<LocationComponent>(e))
		entityManager.removeComponent<LocationComponent>(e);

	LocationComponent locComp{};

	const auto& transComp = entityManager.getSharedComponentOr<TransformComponent>(e);
	glm::vec4 xAxis{ 1.0f, 0.0f, 0.0f, 0.0f };
	xAxis = transComp.m_transform * xAxis;
	auto dirToCheck = OrthogonalDirection::LEFT;
	int nDim{ 0 };
	if (Tools::fEqual(xAxis.x, 1.0f)) {
		dirToCheck = OrthogonalDirection::BOTTOM;
		nDim = 0;
	}
	else if (Tools::fEqual(xAxis.x, -1.0f)) {
		dirToCheck = OrthogonalDirection::TOP;
		nDim = 0;
	}
	else if (Tools::fEqual(xAxis.z, 1.0f)) {
		dirToCheck = OrthogonalDirection::LEFT;
		nDim = 2;
	}
	else if (Tools::fEqual(xAxis.z, -1.0f)) {
		dirToCheck = OrthogonalDirection::RIGHT;
		nDim = 2;
	}
	std::vector<OrthogonalDirection> vDirsToCheck = { dirToCheck, Tools::getOppositeDirection(dirToCheck) };

	auto doLocalizeRecursive = [pRoadSystem, this, e, &vDirsToCheck, &xAxis, nDim](Entity entityToLocalize, LocationComponent& outLoc, GraphNodeComponent::Type& lastType) -> Entity {

		auto [minBounds, maxBounds] = pRoadSystem->getExtents(entityToLocalize);

		//                float fCurX = (float)a->tileX() + .5f;
		//                float fCurZ = (float)a->tileZ() + .5f;

		auto nUpperBoundZ = maxBounds[1];
		auto nLowerBoundZ = minBounds[1];
		auto nUpperBoundX = maxBounds[0];
		auto nLowerBoundX = minBounds[0];

		const auto& transComp = entityManager.getSharedComponentOr<TransformComponent>(entityToLocalize);

		auto mAdjEnts = m_pWorld->getAdjacentEntities(std::make_pair(nLowerBoundX, nLowerBoundZ), std::make_pair(nUpperBoundX, nUpperBoundZ));

		glm::vec4 p{ 0.0f, 0.0f, 0.0f, 1.0f };
		p = transComp.m_transform * p;

		for (auto dir : vDirsToCheck) {
			auto lastEnt = INVALID_ENTITY_ID;
			auto iter = mAdjEnts.find(dir);
			if (iter != mAdjEnts.end()) {
				for (auto ent : iter->second) {
					if (lastEnt == ent)
						continue;
					const auto& otherTransf = entityManager.getSharedComponentOr<TransformComponent>(ent);
					glm::vec4 otherXAxis{ 1.0f, 0.0f, 0.0f, 0.0f };
					otherXAxis = otherTransf.m_transform * otherXAxis;
					if (!Tools::vec_equals(otherXAxis, xAxis) && !Tools::vec_equals(otherXAxis, -xAxis))
						continue;

					const auto& otherMeshRef = entityManager.getComponent<ModelReferenceComponent>(ent);
					const auto& otherMesh = entityManager.getComponent<MeshComponent>(otherMeshRef.m_entity);
					auto meshExt = otherMesh.extents();
					glm::vec4 minPoint{ meshExt.minPoint, 1.0f };
					glm::vec4 maxPoint{ meshExt.maxPoint, 1.0f };
					minPoint = otherTransf.m_transform * minPoint;
					maxPoint = otherTransf.m_transform * maxPoint;
					for (glm::length_t i = 0; i < glm::vec4::length(); ++i) {
						if (minPoint[i] > maxPoint[i])
							std::swap(minPoint[i], maxPoint[i]);
					}
					if (p[nDim] >= minPoint[nDim] && p[nDim] <= maxPoint[nDim]) {
						//std::cout << "Entity " << ent << " is aligned with me" << std::endl;
						if (entityManager.hasComponent<PathSegmentComponent>(ent)) {
							const auto& pathSegComp = entityManager.getComponent<PathSegmentComponent>(ent);
							const auto& pathComp = entityManager.getComponent<PathComponent>(pathSegComp.m_path);
							const auto& edgeComp = entityManager.getComponent<GraphEdgeComponent>(pathSegComp.m_path);
							const auto& nodeComp = entityManager.getComponent<GraphNodeComponent>(edgeComp.m_startNode);

							if (nodeComp.m_type != lastType) {
								lastType = nodeComp.m_type;
								outLoc.m_locations.emplace_back(std::piecewise_construct, std::forward_as_tuple(nodeComp.m_type), std::forward_as_tuple(pathSegComp.m_path, pathComp.m_path.getStationAtSegment(ent)));
								//LocationComponent locComp{};

								//locComp.m_path = pathSegComp.m_path;
								//locComp.m_fStation = pathComp.m_path.getStationAtSegment(ent);
								//entityManager.addComponent(e, locComp);

								if (vDirsToCheck.size() > 1) {
									std::erase_if(vDirsToCheck, [dir](const auto& d) { return d == Tools::getOppositeDirection(dir); });
								}
								std::cout << "Building " << e << " localized at path " << pathSegComp.m_path << ", station " << std::to_string(pathComp.m_path.getStationAtSegment(ent)) << std::endl;
								return ent;
							}
						}
					}
					lastEnt = ent;
				}
			}
		}
		return INVALID_ENTITY_ID;
	};

	Entity startEnt = e;
	GraphNodeComponent::Type lastType{};

	while ((startEnt = doLocalizeRecursive(startEnt, locComp, lastType)) != INVALID_ENTITY_ID);
	entityManager.addComponent(e, locComp);
}

void BuildingSystem::localizeBuildings() noexcept {
	using namespace Ice;

	for (auto e : entities(entityManager.currentScene())) {
		localizeBuilding(e);
	}
}

void BuildingSystem::onEntityAdded(Ice::Entity e) noexcept {
    using namespace Ice;
    
    if (entityManager.hasComponent<BlueprintComponent>(e))
        return;
    
    const auto& buildComp = entityManager.getComponent<BuildingComponent>(e);
    
    using namespace Ice::GUI;
    auto w = systemServices.getWidgetManager().createWidget<GUI::Widget>(nullptr);
    //w->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<GUI::HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    //w->setPointConstraint(VerticalAlignment::TOP, std::make_unique<GUI::VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    w->setSizeConstraint(Ice::OrthogonalAxis::HORIZONTAL, std::make_unique<Ice::GUI::DirectSizeConstraint<Pixels>>(250_px));
    w->setSizeConstraint(Ice::OrthogonalAxis::VERTICAL, std::make_unique<Ice::GUI::DirectSizeConstraint<Pixels>>(100_px));

    ToolTipComponent tooltip;
    tooltip.m_pToolTip = w;

    tooltip.m_getCamFunc = [this]() -> decltype(auto) {
        const auto& camComp = entityManager.getComponent<Ice::CameraComponent>(m_camEnt);
        return camComp;
    };
    entityManager.addComponent(e, tooltip);
    
    auto pHStack = (HorizontalStackPanel*)systemServices.getWidgetManager().createWidget<HorizontalStackPanel>(w);
    pHStack->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<GUI::HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    pHStack->setPointConstraint(VerticalAlignment::TOP, std::make_unique<GUI::VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    pHStack->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pHStack->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
	pHStack->setDefaultHorizontalAlignment(HorizontalAlignment::CENTER);


    auto pVStack1 = (VerticalStackPanel*)systemServices.getWidgetManager().createWidget<VerticalStackPanel>(pHStack);
    pVStack1->setDefaultVerticalAlignment(VerticalAlignment::TOP);
    //pVStack1->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<GUI::HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    //pVStack1->setPointConstraint(VerticalAlignment::TOP, std::make_unique<GUI::VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    pVStack1->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    //pVStack1->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));

    auto pVStack2 = (VerticalStackPanel*)systemServices.getWidgetManager().createWidget<VerticalStackPanel>(pHStack);
    pVStack2->setDefaultVerticalAlignment(VerticalAlignment::TOP);
    //pVStack1->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pVStack2->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));

    auto pDesc = static_cast<GUI::TextLabel*>(systemServices.getWidgetManager().createWidget<GUI::TextLabel>(pVStack1, "Name"));
    pDesc->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pDesc->setHorizontalTextAlignment(HorizontalAlignment::LEFT);

    pDesc = static_cast<GUI::TextLabel*>(systemServices.getWidgetManager().createWidget<GUI::TextLabel>(pVStack2, buildComp.m_strName, false));
    pDesc->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pDesc->setHorizontalTextAlignment(HorizontalAlignment::LEFT);

    pDesc = static_cast<GUI::TextLabel*>(systemServices.getWidgetManager().createWidget<GUI::TextLabel>(pVStack1, "Capacity"));
    pDesc->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pDesc->setHorizontalTextAlignment(HorizontalAlignment::LEFT);

    pDesc = static_cast<GUI::TextLabel*>(systemServices.getWidgetManager().createWidget<GUI::TextLabel>(pVStack2, std::to_string( buildComp.m_nCapacity), false));
    pDesc->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pDesc->setHorizontalTextAlignment(HorizontalAlignment::LEFT);

}

}
