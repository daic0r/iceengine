//
//  RoadNetworkSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 23.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Entities/EntityManager.h>
#include <System/Types.h>
#include <Components/TransformComponent.h>
#include <Components/PathSegmentComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/MeshComponent.h>
#include <Components/GraphEdgeComponent.h>
#include <Components/PathAnimationComponent.h>
#include <Components/LocationComponent.h>
#include <Components/BlueprintComponent.h>
#include <Utils/FinalAction.h>
#include <System/Tools.h>
#include <World/World.h>
#include <glm/gtx/string_cast.hpp>

#include <Components/Systems/RoadNetworkSystem.h>

namespace Ice {

bool RoadNetworkSystem::greater_f_cost::operator()(const RoadNetworkSystem::pathfinding_node& a, const RoadNetworkSystem::pathfinding_node& b) const {
    return a.fCost() > b.fCost() || (a.fCost() == b.fCost() && a.fDistToEnd > b.fDistToEnd);
}

void RoadNetworkSystem::onEntityAdded(Entity) noexcept {
//    readTopology();
}

void RoadNetworkSystem::readTopology() {
    auto mNodes = gatherRelevantEntities();
    
    while (mNodes.size() > 0) {
        Entity e = mNodes.cbegin()->first;
        OrthogonalDirection dir = mNodes.cbegin()->second.front();
        
        auto& nodeComp = entityManager.getComponent<GraphNodeComponent>(e);
        auto constructRes = constructPath(e, dir);
        if (constructRes.m_start != INVALID_ENTITY_ID && constructRes.m_end != INVALID_ENTITY_ID) {
            auto edgeEnt = entityManager.createEntity();
            GraphEdgeComponent edgeComp;
            edgeComp.m_startNode = constructRes.m_start;
            edgeComp.m_endNode = constructRes.m_end;
            edgeComp.m_fLength = constructRes.m_path.m_path.length();

			//constructRes.m_path.m_path.m_myEnt = edgeEnt;

            entityManager.addComponent(edgeEnt, edgeComp);
            entityManager.addComponent(edgeEnt, constructRes.m_path);
            nodeComp.m_vEdges.push_back(edgeEnt);
            entityManager.getComponent<GraphNodeComponent>(constructRes.m_end).m_vEdges.push_back(edgeEnt);
            
            //auto& pathComp = entityManager.getComponent<PathComponent>(e);
            for (auto& [connectDir, pathSegEnt] : constructRes.m_path.m_path.pathSegments()) {
                auto& pathSeg = entityManager.getComponent<PathSegmentComponent>(pathSegEnt);
                pathSeg.m_path = edgeEnt;
            }

			adjacency_info adjInfo;
			adjInfo.endNode = edgeComp.m_endNode;
			adjInfo.edge = edgeEnt;
			adjInfo.dirOnStartNode = Tools::getOppositeDirection(constructRes.m_path.m_path.pathSegments().at(1).first);
			auto& pathSegStart = entityManager.getComponent<PathSegmentComponent>(edgeComp.m_startNode);
			// Transfer exclusions over
			for (const auto& [dir, vExcl] : pathSegStart.m_pathSegment.exclusions()) {
				adjInfo.exclusions.emplace_back(dir, vExcl);
			}
			adjacency_info endNodeInfo;
			endNodeInfo.endNode = edgeComp.m_startNode;
			endNodeInfo.edge = edgeEnt;
			endNodeInfo.dirOnStartNode = (constructRes.m_path.m_path.pathSegments().end() - 1)->first;
			auto& pathSegEnd = entityManager.getComponent<PathSegmentComponent>(edgeComp.m_endNode);
			// Transfer exclusions over
			for (const auto& [dir, vExcl] : pathSegEnd.m_pathSegment.exclusions()) {
				endNodeInfo.exclusions.emplace_back(dir, vExcl);
			}

            m_mAdjacencyMatrix[edgeComp.m_startNode].insert(adjInfo);
            m_mAdjacencyMatrix[edgeComp.m_endNode].insert(endNodeInfo);

            std::cout << "Constructed edge " << edgeEnt << " between entity " << constructRes.m_start << " and " << constructRes.m_end << " of length " <<  edgeComp.m_fLength << std::endl;
        }
        mNodes.begin()->second.erase(mNodes.cbegin()->second.begin());
        if (mNodes.cbegin()->second.size() == 0) {
            mNodes.erase(mNodes.cbegin());
        }
        auto secondNodeIter = mNodes.find(constructRes.m_end);
        if (secondNodeIter != mNodes.cend()) {
            auto dirIter = std::find(secondNodeIter->second.cbegin(), secondNodeIter->second.cend(), constructRes.m_endNodeConnectDir);
            if (dirIter != secondNodeIter->second.cend()) {
                secondNodeIter->second.erase(dirIter);
                if (secondNodeIter->second.size() == 0) {
                    mNodes.erase(secondNodeIter);
                }
            }
        }
    }
    
    for (auto e : entities(entityManager.currentScene())) {
        if (!entityManager.hasComponent<TransformComponent>(e))
            continue;
        std::cout << "Node " << e << " has edges: " << std::endl;
        for (auto edge : entityManager.getComponent<GraphNodeComponent>(e).m_vEdges) {
            std::cout << "\t" << edge << std::endl;
        }
    }
}

std::unordered_map<Entity, std::vector<OrthogonalDirection>> RoadNetworkSystem::gatherRelevantEntities() {
    const auto& ents = entities(entityManager.currentScene());
    std::unordered_map<Entity, std::vector<OrthogonalDirection>> mNodes;
    
    // Gather up all relevant node entities and direction we can travel to from it
    for (Entity e : ents) {
        const auto& nodeComp = entityManager.getComponent<GraphNodeComponent>(e);
        //if (nodeComp.m_type != GraphNodeComponent::Type::PEDESTRIAN)
        //    continue;
        
        // Check if this is a blueprint; if it is, it won't have a transform component
        if (!entityManager.hasComponent<ModelReferenceComponent>(e))
            continue;
        
        auto [iter, bSuccess] = mNodes.emplace(e, std::vector<OrthogonalDirection>{});
        const auto& pathSeg = entityManager.getComponent<PathSegmentComponent>(e);
        for (const auto& [dir, vVerts] : pathSeg.m_pathSegment.vertices())
            iter->second.push_back(dir);
    }
    
    return mNodes;
}

RoadNetworkSystem::sPathConstructResult RoadNetworkSystem::constructPath(Ice::Entity e, Ice::OrthogonalDirection connectDir/* = Ice::OrthogonalDirection::NONE*/) const noexcept {
    using namespace Ice;
    
    sPathConstructResult res;
    res.m_start = e;
    
    if (!entityManager.hasComponent<GraphNodeComponent>(e))
        return res;
    
    auto dir = connectDir;
    auto lastDir = OrthogonalDirection::NONE;
    std::vector<std::pair<OrthogonalDirection, Entity>> vPathSegs;
    
    while (true) {
        
        vPathSegs.push_back(std::make_pair(Tools::getOppositeDirection(lastDir), e));

        std::cout << "Added path segment to path: " << e << ", connects to preceding element on the " << Tools::direction2string(Tools::getOppositeDirection(lastDir)) <<  std::endl;
        
        // Did we find a path segment with a graph node component? -> we're done!
        if (entityManager.hasComponent<GraphNodeComponent>(e) && lastDir != OrthogonalDirection::NONE) {
            res.m_end = e;
            res.m_endNodeConnectDir = Tools::getOppositeDirection(lastDir);
            break;
        }

        auto [minBounds, maxBounds] = getExtents(e);
        
        auto nUpperBoundZ = maxBounds[1];
        auto nLowerBoundZ = minBounds[1];
        auto nUpperBoundX = maxBounds[0];
        auto nLowerBoundX = minBounds[0];
        
        auto mAdjEnts = m_pWorld->getAdjacentEntities(std::make_pair(nLowerBoundX, nLowerBoundZ), std::make_pair(nUpperBoundX, nUpperBoundZ));

        const auto& pathSeg = entityManager.getComponent<PathSegmentComponent>(e);
        const auto& transComp = entityManager.getSharedComponentOr<TransformComponent>(e);

        // If first iteration, grab the first direction
        if (dir == OrthogonalDirection::NONE && lastDir == OrthogonalDirection::NONE) {
            [&dir, &mAdjEnts, pathSeg]() {
                for (const auto& [pathDir, vVerts] : pathSeg.m_pathSegment.vertices()) {
                    for (const auto& [adjDir, vEnts] : mAdjEnts) {
                        if (pathDir == adjDir) {
                            dir = pathDir;
                            return;
                        }
                    }
                }
            }();
            //dir = pathSeg.m_pathSegment.vertices().begin()->first;
        } else if (lastDir != OrthogonalDirection::NONE) {
            bool bFound = false;
            for (const auto& [thisDir, vVerts] : pathSeg.m_pathSegment.vertices()) {
                // Opposite of the last segment's search direction is where we're coming from,
                // so don't check that side
                if (thisDir != Tools::getOppositeDirection(lastDir)) {
                    dir = thisDir;
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
                break;
        }
        
        if (dir == OrthogonalDirection::NONE) {
            if (vPathSegs.size() > 1)
                res.m_path.m_path.setPathSegments(vPathSegs); 
            return res;
        }
//        auto axis = glm::vec4{ Tools::direction2axis(dir), 0.0f };
//        axis = transComp.m_transform * axis;
//        dir = Tools::axis2direction(glm::vec3{ axis });
        
        //auto alignedDir = Tools::axis2direction(xAxis);
        auto oppositeDir = Tools::getOppositeDirection(dir);
		if (mAdjEnts.find(dir) == mAdjEnts.end()) {
			std::cout << "--> No connecting element at my " << Tools::direction2string(dir) << "\n";
			return res;
		}
        bool bAllSame = true;
        Entity lastEnt = INVALID_ENTITY_ID;
        for (auto adjEnt : mAdjEnts.at(dir)) {
            if (lastEnt != INVALID_ENTITY_ID && adjEnt != lastEnt) {
                bAllSame = false;
                break;
            }
            lastEnt = adjEnt;
        }
        if (!bAllSame)
            break;
        if (entityManager.hasComponent<PathSegmentComponent>(lastEnt)) {
            const auto& pathSegNext = entityManager.getComponent<PathSegmentComponent>(lastEnt);
            if (pathSegNext.m_pathSegment.vertices().find(oppositeDir) != pathSegNext.m_pathSegment.vertices().end()) {
                e = lastEnt;
            } else {
                break;
            }
        }
        lastDir = dir;
    }
    
    std::cout << "Done searching path." << std::endl;
    if (vPathSegs.size() > 1)
        res.m_path.m_path.setPathSegments(vPathSegs);
    return res;
}

/// TO BE MOVED -> EXTENTS CALCULATION SHOULD BE DONE ELSEWHERE

std::pair<glm::ivec2, glm::ivec2> RoadNetworkSystem::getExtents(Ice::Entity e) const {
    using namespace Ice;

	auto getEntityExtents = [this](Entity e) {
		const auto& refComp = entityManager.getComponent<ModelReferenceComponent>(e);
		const auto& meshComp = entityManager.getComponent<MeshComponent>(refComp.m_entity);
		const auto& transComp = entityManager.getSharedComponentOr<TransformComponent>(e);
		glm::vec4 minPoint{ meshComp.extents().minPoint, 1.0f };
		glm::vec4 maxPoint{ meshComp.extents().maxPoint, 1.0f };

		glm::mat3 rotTrans3{ transComp.m_transform };
		glm::mat4 rotTrans4{ rotTrans3 };
		minPoint = rotTrans4 * minPoint;
		maxPoint = rotTrans4 * maxPoint;
		if (minPoint.x > maxPoint.x)
			std::swap(minPoint.x, maxPoint.x);
		if (minPoint.z > maxPoint.z)
			std::swap(minPoint.z, maxPoint.z);

		float fMinX = minPoint.x;
		float fMinZ = minPoint.z;
		float fMaxX = maxPoint.x;
		float fMaxZ = maxPoint.z;

		auto fCurX = transComp.m_transform[3][0];
		auto fCurZ = transComp.m_transform[3][2];

		int nFromX = (int)std::round(fCurX + fMinX);
		int nToX = (int)std::round(fCurX + fMaxX) - 1;
		int nFromZ = (int)std::round(fCurZ + fMinZ);
		int nToZ = (int)std::round(fCurZ + fMaxZ) - 1;

		glm::ivec2 retMin, retMax;
		retMax.y = std::min((int)m_pWorld->worldSize(), nToZ);
		retMin.y = std::max(0, nFromZ);
		retMax.x = std::min((int)m_pWorld->worldSize(), nToX);
		retMin.x = std::max(0, nFromX);
		return std::make_pair(retMin, retMax);
	};
    
	if (entityManager.hasComponent<ModelReferenceComponent>(e))
		return getEntityExtents(e);
        
    std::pair<glm::ivec2, glm::ivec2> ret{ std::numeric_limits<int>::max(), std::numeric_limits<int>::min() };
    const auto& lec = entityManager.getComponent<LinkedEntitiesComponent>(e);
        
    for (auto meshEnt : lec.m_sLinkedEntities) {
        auto [locMin, locMax] = getEntityExtents(meshEnt);
        if (locMin.x < ret.first.x)
            ret.first.x = locMin.x;
        if (locMin.y < ret.first.y)
            ret.first.y = locMin.y;
            
        if (locMax.x > ret.second.x)
            ret.second.x = locMax.x;
        if (locMax.y > ret.second.y)
            ret.second.y = locMax.y;
    }
        
    return ret;
}

/*
 * \name findPath
 *
 * \date 09.02.2021
 * \param const LocationComponent& start: topology location from which to find the path
 * \param const LocationComponent& end  : topology location where the path ends
 *
 * \description Performs path finding between two locations on the topology. Picks the
 *              start nodes of the edges associated with the locations and performs regular
 *              A* path finding. If the edges the locations are on are missing from the resulting
 *              sequence of nodes, they are added to the sequence.
 *
 */
std::vector<Entity> RoadNetworkSystem::findPath(const LocationComponent& start, const LocationComponent& end, GraphNodeComponent::Type t) {
	auto startIter = std::find_if(start.m_locations.cbegin(), start.m_locations.cend(), [t](const auto& kvp) {
		return kvp.first == t;
	});
	if (startIter == start.m_locations.end())
		startIter = start.m_locations.begin();
	auto endIter = std::find_if(end.m_locations.cbegin(), end.m_locations.cend(), [t](const auto& kvp) {
		return kvp.first == t;
	});
	if (endIter == end.m_locations.end())
		endIter = end.m_locations.begin();
    const auto& startEdge = entityManager.getComponent<GraphEdgeComponent>(startIter->second.m_path);
    const auto& endEdge = entityManager.getComponent<GraphEdgeComponent>(endIter->second.m_path);
	const auto& pathComp = entityManager.getComponent<PathComponent>(endIter->second.m_path);
	Entity endSegEnt = pathComp.m_path.getSegmentAtStation(endIter->second.m_fStation);
    auto startNodeEnt = startEdge.m_startNode;

	/////////////////////////////////////////////////
	// Make sure start edge is included!
	const auto& sStartNodeNeighbors = m_mAdjacencyMatrix.at(startEdge.m_startNode);
	const auto& startPathSegment = entityManager.getComponent<PathSegmentComponent>(startEdge.m_startNode);
	auto toOtherNodeOnStartEdgeIter = std::find_if(sStartNodeNeighbors.cbegin(), sStartNodeNeighbors.cend(), [&startEdge](const adjacency_info& i) {
		return i.endNode == startEdge.m_endNode;
		});
	auto orient = startPathSegment.m_pathSegment.edgeOrientation(toOtherNodeOnStartEdgeIter->dirOnStartNode);
	Entity firstPrevNode = INVALID_ENTITY_ID;
	// We have to make sure that we only travel in a legal direction on the start edge!
	if (((orient == EdgeOrientation::WITH_PATH) && (toOtherNodeOnStartEdgeIter->dirOnStartNode == OrthogonalDirection::LEFT || toOtherNodeOnStartEdgeIter->dirOnStartNode == OrthogonalDirection::BOTTOM))
		||
		(orient == EdgeOrientation::AGAINST_PATH && (toOtherNodeOnStartEdgeIter->dirOnStartNode == OrthogonalDirection::RIGHT || toOtherNodeOnStartEdgeIter->dirOnStartNode == OrthogonalDirection::TOP))
		)
	{
		firstPrevNode = startEdge.m_endNode;
	}
	/////////////////////////////////////////////////


	/////////////////////////////////////////////////
	// Sub-functions used in the algorithm
    auto nodeGreater = [](const RoadNetworkSystem::pathfinding_node& a, const RoadNetworkSystem::pathfinding_node& b) {
        return a.fCost() > b.fCost() || (a.fCost() == b.fCost() && a.fDistToEnd > b.fDistToEnd);
    };
    auto getDistance = [](Entity a, Entity b) -> float {
        glm::vec3 pointA;
        glm::vec3 pointB;
        entityManager.getSharedComponentOr<TransformComponent>(a, [&pointA](const TransformComponent& transfA) {
            pointA = glm::vec3{ transfA.m_transform[3][0], transfA.m_transform[3][1], transfA.m_transform[3][2] };
        });

        entityManager.getSharedComponentOr<TransformComponent>(b, [&pointB](const TransformComponent& transfB) {
            pointB = glm::vec3{ transfB.m_transform[3][0], transfB.m_transform[3][1], transfB.m_transform[3][2] };
        });
        return glm::length(pointA - pointB);
    };
	auto canTraverseEdgeFrom = [](const std::set<adjacency_info>& sNeighborNodes, const adjacency_info& adjInfo, Entity from) -> bool {
		// Do we have exclusions on traveling in direction (adjInfo.dirOnStartNode) on this node to the next next node?
		auto exclDirIter = std::find_if(adjInfo.exclusions.cbegin(), adjInfo.exclusions.cend(), [adjInfo](const auto& kvp) {
			return kvp.first == adjInfo.dirOnStartNode;
			});
		// If we don't, bail
		if (exclDirIter == adjInfo.exclusions.cend())
			return true;

		// Get info on edge (this node -> previous node)
		auto prevConIter = std::find_if(sNeighborNodes.cbegin(), sNeighborNodes.cend(), [from](const adjacency_info& info) {
				return info.endNode == from;
			});
		auto fromDir = prevConIter->dirOnStartNode;
		// Is the direction we're coming from part of the excluded source directions?
		auto bIsExcluded = std::any_of(exclDirIter->second.cbegin(), exclDirIter->second.cend(), [fromDir](OrthogonalDirection d) { return d == fromDir; });
		return !bIsExcluded;
	};
	/////////////////////////////////////////////////

    std::vector<Entity> vRet;

    m_vOpenSet.clear();
	m_vClosedSet.clear();
    m_vOpenSet.emplace_back(startNodeEnt);
	if (firstPrevNode != INVALID_ENTITY_ID) {
		m_vOpenSet.back().prevNode = firstPrevNode;
		m_vClosedSet.push_back(firstPrevNode);
	}
    while (!m_vOpenSet.empty()) {
        auto node = m_vOpenSet.back();
        m_vOpenSet.pop_back();
        m_vClosedSet.push_back(node);
	    const auto& sNeighborNodes = m_mAdjacencyMatrix.at(node.graphNode);
        if (node.graphNode == endEdge.m_startNode || node.graphNode == endEdge.m_endNode) {

			// Check if we can traverse to the other end of the final edge or if we're restricted by exclusions
			Entity otherNode = node.graphNode == endEdge.m_startNode ? endEdge.m_endNode : endEdge.m_startNode;
			auto endNodeIter = std::find_if(sNeighborNodes.cbegin(), sNeighborNodes.cend(),[otherNode](const adjacency_info& info) {
				return info.endNode == otherNode;
			});
			if (canTraverseEdgeFrom(sNeighborNodes, *endNodeIter, node.prevNode)) {
				// Traverse back to the beginning by following the previous node entry
				while (true) {
					vRet.push_back(node.graphNode);
					if (node.prevNode == INVALID_ENTITY_ID)
						break;
					auto iter = std::find_if(m_vClosedSet.cbegin(), m_vClosedSet.cend(), [&node](const pathfinding_node& n) {
						return n.graphNode == node.prevNode;
						});
					if (iter != m_vClosedSet.end())
						node = *iter;
				};
				std::reverse(vRet.begin(), vRet.end());
				break;
			}
        }

		const auto& nodeComp = entityManager.getComponent<GraphNodeComponent>(node.graphNode);
        for (const auto& adjInfo : sNeighborNodes) {
			Entity neighborNodeEnt = adjInfo.endNode;
			Entity edgeEnt = adjInfo.edge;
			
			// Prevent crossing over to other nodes for now to prevent driving on sidewalks
			const auto& neighborNodeComp = entityManager.getComponent<GraphNodeComponent>(neighborNodeEnt);
			if (neighborNodeComp.m_type != nodeComp.m_type)
				continue;

            auto findNeighborEntFunc = [neighborNodeEnt=neighborNodeEnt](const pathfinding_node& n) { return neighborNodeEnt == n.graphNode; };
            if (std::find_if(m_vClosedSet.cbegin(), m_vClosedSet.cend(), findNeighborEntFunc) != m_vClosedSet.cend())
                continue;

            float fCostToNeighbor;
            entityManager.getComponent<GraphEdgeComponent>(edgeEnt, [&fCostToNeighbor, &node](const GraphEdgeComponent& comp) {
                fCostToNeighbor = node.fDistFromStart + comp.m_fLength;
            });
            auto neighborIter = std::find_if(m_vOpenSet.begin(), m_vOpenSet.end(), findNeighborEntFunc);
            const bool bMustUpdateNeighbor = (neighborIter == m_vOpenSet.end()) || (fCostToNeighbor < neighborIter->fDistFromStart);
			if (bMustUpdateNeighbor) {
				const bool bCanUseNode = canTraverseEdgeFrom(sNeighborNodes, adjInfo, node.prevNode);
				if (!bCanUseNode) {
					std::cout << ">> Skipping direction " << Tools::direction2string(adjInfo.dirOnStartNode) << " on node " << node.graphNode << "\n";
				}
				else {
					if (neighborIter == m_vOpenSet.end()) {
						neighborIter = m_vOpenSet.insert(m_vOpenSet.end(), pathfinding_node{ neighborNodeEnt });
						neighborIter->fDistToEnd = getDistance(neighborNodeEnt, endSegEnt);
					}
					neighborIter->fDistFromStart = fCostToNeighbor;
					neighborIter->prevNode = node.graphNode;
				}
            }
            if (m_vOpenSet.size() > 1)
                std::sort(m_vOpenSet.begin(), m_vOpenSet.end(), nodeGreater);
        } // end for each neighbor


	}
	if (vRet.size() < 2)
		return vRet;
//    const bool bStartEdgeIncluded = (vRet.at(0) == startEdge.m_startNode && vRet.at(1) == startEdge.m_endNode) ||
//        (vRet.at(1) == startEdge.m_startNode && vRet.at(0) == startEdge.m_endNode);
    const bool bEndEdgeIncluded = (vRet.at(vRet.size() - 1) == endEdge.m_startNode && vRet.at(vRet.size() - 2) == endEdge.m_endNode) ||
        (vRet.at(vRet.size() - 2) == endEdge.m_startNode && vRet.at(vRet.size() - 1) == endEdge.m_endNode);

    //if (!bStartEdgeIncluded) {
    //    auto entToAdd = vRet.at(0) == startEdge.m_startNode ? startEdge.m_endNode : startEdge.m_startNode;
    //    vRet.insert(vRet.begin(), entToAdd);
    //}
    if (!bEndEdgeIncluded) {
        auto entToAdd = vRet.at(vRet.size() - 1) == endEdge.m_startNode ? endEdge.m_endNode : endEdge.m_startNode;
        vRet.push_back(entToAdd);
    }
    return vRet;
}

/*
 * \name getPathAnimationFromNodeList
 *
 * \date 09.02.2021
 * \param const std::vector<Entity>& vList: sequence of nodes from which to create a path animation
 * \param float fStartPos                 : station on the first edge where to start the animation
 * \param float fTargetPos                : station on the last edge where to start the animation
 *
 * \description Creates a PathAnimationComponent from the passed in parameters. fStartPos and fTargetPos
 *              indicate the >station< on the first and last edge, i.e. the position from the start node of
 *              the edge. This is different from the m_fCurrentPosition and m_fTargetPosition in the animation
 *              component, since these are relative to the direction of the animation. Therefore at the end
 *              a conversion is performed.
 *
 */
PathAnimationComponent RoadNetworkSystem::getPathAnimationFromNodeList(const std::vector<Entity> &vList, float fStartPos, float fTargetPos) const noexcept {
    PathAnimationComponent ret{};
    
    ret.m_fSpeed = 2.f;
    for (size_t i = 0; i < vList.size() - 1; ++i) {
        const auto startEnt = vList[i];
        const auto endEnt = vList[i + 1];
        const auto& adjMatEntry = m_mAdjacencyMatrix.at(startEnt);
        const auto& adjInfo = *std::find_if(adjMatEntry.cbegin(), adjMatEntry.cend(), [endEnt](const auto& adjInfo) {
            return adjInfo.endNode == endEnt;
        });
        
		Entity edgeEnt = adjInfo.edge;

        entityManager.getComponent<GraphEdgeComponent>(edgeEnt, [edgeEnt=edgeEnt, startEnt, endEnt, &ret](const GraphEdgeComponent& edgeComp) {
            entityManager.getComponent<PathComponent>(edgeEnt, [edgeEnt, &edgeComp, startEnt, endEnt, &ret](const PathComponent& pathComp) {
                const bool bInverse = edgeComp.m_startNode == endEnt && edgeComp.m_endNode == startEnt;
                ret.m_vPaths.emplace_back(edgeEnt, bInverse);
            });
        });
        
    }
    const auto& firstPath = ret.m_vPaths.front();
    const auto& lastPath = ret.m_vPaths.back();
    // Current position is always the path-local position starting from the
    // node the animation is coming from
    ret.m_fCurrentPosition = firstPath.second ? entityManager.getComponent<PathComponent>(firstPath.first).m_path.length() - fStartPos : fStartPos;
    ret.m_fTargetPosition = lastPath.second ? entityManager.getComponent<PathComponent>(lastPath.first).m_path.length() - fTargetPos : fTargetPos;

	const auto& nodeComp = entityManager.getComponent<GraphNodeComponent>(vList[0]);
	ret.m_nodeType = nodeComp.m_type;
    return ret;
}

}
