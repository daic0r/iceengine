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
            BOTTOM_RIGHT_FRONT,
            BOTTOM_RIGHT_BACK,
            BOTTOM_LEFT_BACK,
            TOP_LEFT_FRONT,
            TOP_RIGHT_FRONT,
            TOP_RIGHT_BACK,
            TOP_LEFT_BACK
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
                return kvp.second;
            });
        }
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
        return intersects_impl(ray, vOut, m_root.get());
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