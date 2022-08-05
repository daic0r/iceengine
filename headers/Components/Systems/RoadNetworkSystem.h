//
//  RoadNetworkSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 23.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef RoadNetworkSystem_h
#define RoadNetworkSystem_h

#include <IceEngine.h>
#include <unordered_map>

#include <Entities/EntityComponentSystem.h>
#include <Components/GraphNodeComponent.h>
#include <Components/PathComponent.h>

namespace Ice {

class World;
struct LocationComponent;
struct PathAnimationComponent;

class ICEENGINE_API RoadNetworkSystem : public EntityComponentSystem<GraphNodeComponent> {
	struct adjacency_info;
    std::unordered_map<Entity, std::set<adjacency_info>> m_mAdjacencyMatrix; // start node -> (end node, edge, exclusions)

	struct adjacency_info {
		Entity endNode;
		Entity edge;
		OrthogonalDirection dirOnStartNode;
		std::vector<std::pair<OrthogonalDirection, std::vector<OrthogonalDirection>>> exclusions;

		bool operator<(const adjacency_info& rhs) const { return endNode < rhs.endNode; }
	};
    struct pathfinding_node {
        Entity graphNode{ 0 };
        Entity prevNode{ 0 };
        float fDistFromStart{ 0.0f };  // g cost
        float fDistToEnd{ 0.0f }; // h cost

        float fCost() const noexcept { return fDistFromStart + fDistToEnd; }
        
        pathfinding_node(Entity g = 0, Entity p = 0, float distStart = 0.0f, float distEnd = 0.0f) : graphNode{ g }, prevNode{ p }, fDistFromStart{ distStart }, fDistToEnd{ distEnd } {}
    };
    struct greater_f_cost {
        bool operator()(const pathfinding_node&, const pathfinding_node&) const;
    };    
    std::vector<pathfinding_node> m_vOpenSet;
    std::vector<pathfinding_node> m_vClosedSet;

    World* m_pWorld{ nullptr };
    
    struct sPathConstructResult {
        PathComponent m_path;
        Entity m_start{ INVALID_ENTITY_ID }, m_end{ INVALID_ENTITY_ID };
        OrthogonalDirection m_endNodeConnectDir{ OrthogonalDirection::NONE };
    };

public:

    void onEntityAdded(Entity) noexcept override;
    inline void setWorld(World* w) noexcept { m_pWorld = w; }
    std::pair<glm::ivec2, glm::ivec2> getExtents(Entity) const;
    sPathConstructResult constructPath(Ice::Entity e, Ice::OrthogonalDirection connectDir) const noexcept;
    void readTopology();
    //std::vector<Entity> findPath(const LocationComponent&, const LocationComponent&) const noexcept;
    std::vector<Entity> findPath(const LocationComponent& start, const LocationComponent& end, GraphNodeComponent::Type);
    PathAnimationComponent getPathAnimationFromNodeList(const std::vector<Entity>&, float fStartPos = 0.0f, float fTargetPos = -1.0f) const noexcept;

private:
    std::unordered_map<Entity, std::vector<OrthogonalDirection>> gatherRelevantEntities();
};

}

#endif /* RoadNetworkSystem_h */
