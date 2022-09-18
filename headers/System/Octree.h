#ifndef Octree_h
#define Octree_h

#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <System/Math.h>
#include <System/AABB.h>
#include <ranges>
#include <variant>
#include <System/Ray.h>
#include <Utils/TemplateHelpers.h>
#include <memory>

namespace Ice
{
    template<typename T>
    class Octree {
        enum NodePosition {
            BOTTOM_LEFT_FRONT,
            BOTTOM_LEFT_BACK,
            TOP_LEFT_FRONT,
            TOP_LEFT_BACK,
            BOTTOM_RIGHT_FRONT,
            BOTTOM_RIGHT_BACK,
            TOP_RIGHT_FRONT,
            TOP_RIGHT_BACK,
            EXIT
        };

        enum class Plane {
            XY,
            XZ,
            YZ
        };

        struct sIntersectParams {
            std::array<float, 3> t0; // entry point along ray in each dimension
            std::array<float, 3> t1; // exit point along ray in each dimension
            std::array<float, 3> tm; // center point along ray in each dimension

        };
        struct leaf_node;
        struct branch_node;

        using node_t = std::variant<branch_node, leaf_node>;

        struct base_node {
            AABB box;
        };
        struct leaf_node : base_node {
            std::vector<T> vObjects;
        };
        struct branch_node : base_node {
            std::array<std::unique_ptr<node_t>, 8> arNodes;
        };

        void construct(const std::vector<std::pair<AABB, T>>&, std::unique_ptr<node_t>& node, const AABB& originalBox);
        bool intersects_impl(const Ray&, std::vector<T>& vOut, const node_t* node) const;
        node_t* find(const glm::vec3& point, node_t* pStart = nullptr) const noexcept;

        static int getFirstSubNodeIndex(const sIntersectParams& params) noexcept;
        static NodePosition getNextSubNode(NodePosition current, Plane exitPlane);
        auto transformRayAndGetNodeMappingFunction(Ray&) const noexcept;
    public:
        Octree() = default;
        Octree(const std::vector<std::pair<AABB, T>>& vBoxes, const AABB& outerBox);
        bool intersects(const Ray&, std::vector<T>& vOut) const;
        void emplace(const AABB& box,const T& val);

    private:

        std::unique_ptr<node_t> m_root;
   
    };
    
    template<typename T>
    Octree<T>::Octree(const std::vector<std::pair<AABB, T>>& vBoxes, const AABB& outerBox) {
        construct(vBoxes, m_root, outerBox);
    }

    template<typename T>
    void Octree<T>::construct(const std::vector<std::pair<AABB, T>>& vPoints, std::unique_ptr<node_t>& node, const AABB& originalBox) {
        static const auto getCenterAlongDim = [](const AABB& box, int nDim) {
            return box.minVertex()[nDim] + (box.maxVertex()[nDim] - box.minVertex()[nDim]) / 2.0f;
        };

        struct sNodeInfo {
            std::vector<std::pair<AABB, T>> vNodeContents;
            AABB box;
        };
        std::array<sNodeInfo, 8> arSubNodes;
        bool bAllHaveContent = true;
        bool bOneHasAll = false;
        for (int i = 0; i < 8; ++i) {
            AABB box = originalBox;
            switch (i) {
                case BOTTOM_LEFT_FRONT:
                    box.maxVertex().x = getCenterAlongDim(originalBox, 0);
                    box.maxVertex().y = getCenterAlongDim(originalBox, 1);
                    box.minVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                 case BOTTOM_RIGHT_FRONT:
                    box.minVertex().x = getCenterAlongDim(originalBox, 0);
                    box.maxVertex().y = getCenterAlongDim(originalBox, 1);
                    box.minVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                case BOTTOM_LEFT_BACK:
                    box.maxVertex().x = getCenterAlongDim(originalBox, 0);
                    box.maxVertex().y = getCenterAlongDim(originalBox, 1);
                    box.maxVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                 case BOTTOM_RIGHT_BACK:
                    box.minVertex().x = getCenterAlongDim(originalBox, 0);
                    box.maxVertex().y = getCenterAlongDim(originalBox, 1);
                    box.maxVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                case TOP_LEFT_FRONT:
                    box.maxVertex().x = getCenterAlongDim(originalBox, 0);
                    box.minVertex().y = getCenterAlongDim(originalBox, 1);
                    box.minVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                 case TOP_RIGHT_FRONT:
                    box.minVertex().x = getCenterAlongDim(originalBox, 0);
                    box.minVertex().y = getCenterAlongDim(originalBox, 1);
                    box.minVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                case TOP_LEFT_BACK:
                    box.maxVertex().x = getCenterAlongDim(originalBox, 0);
                    box.minVertex().y = getCenterAlongDim(originalBox, 1);
                    box.maxVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                 case TOP_RIGHT_BACK:
                    box.minVertex().x = getCenterAlongDim(originalBox, 0);
                    box.minVertex().y = getCenterAlongDim(originalBox, 1);
                    box.maxVertex().z = getCenterAlongDim(originalBox, 2);
                    break;
                default:
                    throw std::logic_error("Invalid index");
            }
            arSubNodes[i].box = box;
            std::ranges::copy_if(vPoints, std::back_inserter(arSubNodes[i].vNodeContents), [&box](const auto& elem) {
                return box.intersects(elem.first);
            });

            if (arSubNodes[i].vNodeContents.empty()) {
                bAllHaveContent = false;
                break;
            }
            if (arSubNodes[i].vNodeContents.size() == vPoints.size()) {
                bOneHasAll = true;
                break;
            }
        }

        node = std::make_unique<node_t>();
        if (bAllHaveContent && !bOneHasAll) {
            *node = branch_node{};
            auto& thisNode = std::get<branch_node>(*node);
            thisNode.box = originalBox;
            for (int i = 0; i < 8; ++i) {
                construct(arSubNodes[i].vNodeContents, thisNode.arNodes[i], arSubNodes[i].box);
            }
        }
        else {
            *node = leaf_node{};
            auto& thisNode = std::get<leaf_node>(*node);
            thisNode.box = originalBox;
            auto& v = thisNode.vObjects;
            std::ranges::transform(vPoints, std::back_inserter(v), [](const std::pair<AABB, T>& kvp) {
                const auto bIsFirst = kvp.second[0].x == 0 && kvp.second[1].x == 0 && kvp.second[2].x == 15 && kvp.second[0].z == 0 && kvp.second[1].z == 15 && kvp.second[2].z == 0;
                const auto bIsSecond = kvp.second[0].x == 15 && kvp.second[1].x == 15 && kvp.second[2].x == 30 && kvp.second[0].z == 0 && kvp.second[1].z == 15 && kvp.second[2].z == 15;
                return kvp.second;
            });
        }
    }

    template<typename T>
    Octree<T>::node_t* Octree<T>::find(const glm::vec3& point, node_t* pStart) const noexcept {
        auto pCurNode = pStart ? pStart : m_root.get();
        if (!std::visit([&point](const auto& start) { return start.box.contains(point); }, *pCurNode))
            return nullptr;
        node_t* pRet{};
        while (pCurNode) {
            std::visit(visitor {
                [&point,&pCurNode](const branch_node& branch) {
                    for (const auto& pSubNode : branch.arNodes) {
                        const auto bContains = std::visit( 
                            [&point](const auto& subNode) {
                                return subNode.box.contains(point);
                            }
                        , *pSubNode);
                        if (bContains) {
                            pCurNode = pSubNode.get();
                            return;
                        }
                    }
                    throw std::logic_error("Point contained by surrounding box but not by any of the contained ones!");
                },
                [&pRet,&pCurNode](const leaf_node& leaf){
                    pRet = pCurNode;
                    pCurNode = nullptr;
                } 
            },  *pCurNode);
        }
        return pRet;
    }

    template<typename T>
    bool Octree<T>::intersects_impl(const Ray& ray, std::vector<T>& vOut, const node_t* node) const {
        auto bRet = std::visit(visitor {
            [&ray,&vOut,this](const branch_node& curNode) { 
                for (const auto& pSubNode : curNode.arNodes) {
                    const auto bIntersect = std::visit(
                        [&ray](const auto& n) {
                            return n.box.intersects(ray);
                        }
                    , *pSubNode);
                    if (bIntersect) {
                        return intersects_impl(ray, vOut, pSubNode.get());
                    }
                }
            throw std::logic_error("Ray intersections surrounding box but none of the inner ones");
            },
            [&vOut](const leaf_node& leaf) {
                vOut.insert(vOut.end(), leaf.vObjects.begin(), leaf.vObjects.end());
                return !vOut.empty();
            }
        }, *node);
        return bRet;
    }

    template<typename T>
    bool Octree<T>::intersects(const Ray& ray, std::vector<T>& vOut) const {
        if (!std::get<branch_node>(*m_root).box.intersects(ray))
            return false;
        auto pStart = find(ray.origin());
        bool bHitLeaf{};
        return intersects_impl(ray, vOut, pStart ? pStart : m_root.get());
    }

     
    template<typename T>
    int Octree<T>::getFirstSubNodeIndex(const sIntersectParams& params) noexcept {
        int nRetOctant{};
        const auto maxt0dim = static_cast<Plane>(std::distance(params.t0.begin(), std::ranges::max_element(params.t0)));
        switch (maxt0dim) {
            case Plane::XY:
                nRetOctant |= 1 * static_cast<int>(params.tm[0] < params.t0[2]); 
                nRetOctant |= 2 * static_cast<int>(params.tm[1] < params.t0[2]); 
                break;
            case Plane::XZ:
                nRetOctant |= 1 * static_cast<int>(params.tm[0] < params.t0[1]); 
                nRetOctant |= 4 * static_cast<int>(params.tm[2] < params.t0[1]); 
                break; 
            case Plane::YZ:
                nRetOctant |= 2 * static_cast<int>(params.tm[1] < params.t0[0]); 
                nRetOctant |= 4 * static_cast<int>(params.tm[2] < params.t0[0]); 
                break;
        }
        return nRetOctant;
    }

    template<typename T>
    Octree<T>::NodePosition Octree<T>::getNextSubNode(NodePosition current, Plane exitPlane) {
        switch (current) {
            case NodePosition::BOTTOM_LEFT_FRONT:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::BOTTOM_RIGHT_FRONT;
                        case Plane::XZ:
                            return NodePosition::TOP_LEFT_FRONT;
                        case Plane::XY:
                            return NodePosition::BOTTOM_LEFT_BACK;
                    }
                }
                break;
            case NodePosition::BOTTOM_LEFT_BACK:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::BOTTOM_RIGHT_BACK;
                        case Plane::XZ:
                            return NodePosition::TOP_LEFT_BACK;
                        case Plane::XY:
                            return NodePosition::EXIT;
                    }
                }
                break;
            case NodePosition::TOP_LEFT_FRONT:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::TOP_RIGHT_FRONT;
                        case Plane::XZ:
                            return NodePosition::EXIT;
                        case Plane::XY:
                            return NodePosition::TOP_LEFT_BACK;
                    }
                }
                break;
            case NodePosition::TOP_LEFT_BACK:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::TOP_RIGHT_BACK;
                        case Plane::XZ:
                            return NodePosition::EXIT;
                        case Plane::XY:
                            return NodePosition::EXIT;
                    }
                }
                break;
            case NodePosition::BOTTOM_RIGHT_FRONT:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::EXIT;
                        case Plane::XZ:
                            return NodePosition::TOP_RIGHT_FRONT;
                        case Plane::XY:
                            return NodePosition::BOTTOM_RIGHT_BACK;
                    }
                }
                break;
            case NodePosition::BOTTOM_RIGHT_BACK:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::EXIT;
                        case Plane::XZ:
                            return NodePosition::TOP_RIGHT_BACK;
                        case Plane::XY:
                            return NodePosition::EXIT;
                    }
                }
                break;
            case NodePosition::TOP_RIGHT_FRONT:
                {
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::EXIT;
                        case Plane::XZ:
                            return NodePosition::EXIT;
                        case Plane::XY:
                            return NodePosition::TOP_RIGHT_BACK;
                    }
                }
                break;
            case NodePosition::TOP_RIGHT_BACK:
            case NodePosition::EXIT:
                return NodePosition::EXIT;
        }
        throw std::logic_error("Invalid node position");
    }

    template<typename T>
    auto Octree<T>::transformRayAndGetNodeMappingFunction(Ray& ray) const noexcept {
        auto dir = ray.direction();
        auto origin = ray.origin();
        int nXORMask{};
        for (glm::vec3::length_type i = 0; i < 3; ++i) {
            if (dir[i] < 0.0f) {
                dir[i] *= -1.0f;
                origin[i] = std::get<branch_node>(*m_root).box[i].size(i) - origin[i];
                nXORMask |= (1 << (3-i-1));
            }
        }
        ray.setOrigin(origin);
        ray.setDirection(dir);
        return [nXORMask](int nNodePosition) {
            return nNodePosition ^ nXORMask;
        };
    }

    template<typename T>
    void Octree<T>::emplace(const AABB& box, const T& value) {
        auto curNode = m_root.get();
        bool bDone{};
        while (!bDone) {
           std::visit(visitor {
                [&box,&value,&curNode](const branch_node& node) { 
                    bool bFound{};
                    for (const auto& pSubNode : node.arNodes) {
                        const auto bIntersect = std::visit(
                            [&box](const auto& n) {
                                return n.box.contains(box);
                            }
                        , *pSubNode);
                        if (bIntersect) {
                            curNode = pSubNode.get();
                            bFound = true;
                        }
                    }
                    if (!bFound)
                        throw std::logic_error("No fitting box found!");
                },
                [&value,&bDone](leaf_node& leaf) {
                    leaf.vObjects.push_back(value); 
                    bDone = true; 
                }
            }, *curNode);
        }
    }
} // namespace Ice


#endif