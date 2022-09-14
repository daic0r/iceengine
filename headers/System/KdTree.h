#ifndef KdTree_h
#define KdTree_h

#include <unordered_set>
#include <vector>
#include <glm/vec3.hpp>
#include <variant>
#include <type_traits>
#include <System/AABB.h>
#include <limits>
#include <Utils/TemplateHelpers.h>
#include <functional>
#include <Renderer/Frustum.h>
#include <System/Ray.h>
#include <Entities/Entity.h>
#include <ranges>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <System/Math.h>

namespace Ice
{
    class Frustum;
    class Ray;

    template<typename LeafNodeContainerType, typename ValueType>
    class KdTree {
    public:
        struct leaf_node;
        struct branch_node;
        using node_t = std::variant<branch_node, leaf_node>;
        using collection_func_t = std::function<void(const LeafNodeContainerType&)>;
        using intersects_collection_func_t = std::function<bool(const Ray&, const LeafNodeContainerType&)>;
        using emplace_func_t = std::function<void(LeafNodeContainerType&, const ValueType&)>;

        struct leaf_node {
           LeafNodeContainerType m_container;
        };
        struct branch_node : leaf_node {
            float m_fLocation{}; 
            node_t* m_pLeft{}, *m_pRight{};
        };
   public:
        KdTree() = default;
        KdTree(std::vector<glm::vec3> vPoints);
        void construct(std::vector<glm::vec3> vPoints);
        void print(node_t* pNode = nullptr, int nAxis = 0);
        template<typename U = ValueType>
        void emplace(const glm::vec3& p, U&& u);
        void getVisibleObjects(const Frustum*) const;
        bool intersects(const Ray&) const;
        const auto& boundingBox() const noexcept { return m_outerBox; }
        void setGetVisibleObjectCollectionFunc(collection_func_t f) { m_getVisibleObjectCollectionFunc = std::move(f); }
        void setIntersectsCollectionFunc(intersects_collection_func_t f) { m_intersectsCollectionFunc = std::move(f); }
        void setEmplaceFunc(emplace_func_t f) { m_emplaceFunc = std::move(f); }
   private:
        node_t* subdivide(std::vector<glm::vec3>, int nAxis, int nLevel = 0);
        void getVisibleObjects_impl(const Frustum*, const AABB& box, node_t* pCurNode, int nAxis) const;
        bool intersects(const Ray&, node_t* pCurNode, const AABB& box, int nAxis) const;

        node_t* m_pRoot{};
        std::vector<node_t> m_vNodes{};
        AABB m_outerBox{};
        collection_func_t m_getVisibleObjectCollectionFunc;
        intersects_collection_func_t m_intersectsCollectionFunc;
        emplace_func_t m_emplaceFunc;
    };
    

    template<typename LeafNodeContainerType, typename ValueType>
    template<typename U>
    void KdTree<LeafNodeContainerType, ValueType>::emplace(const glm::vec3& p, U&& u) {
        auto pCurNode = m_pRoot;
        int nAxis{};
        while (pCurNode) {
            std::visit(visitor{ 
                [this,&p,&pCurNode,nAxis,obj=std::forward<U>(u)](branch_node& branch) {
                    this->m_emplaceFunc(branch.m_container, obj);
                    pCurNode = p[nAxis] <= branch.m_fLocation ? branch.m_pLeft : branch.m_pRight;
                },
                [this,obj=std::forward<U>(u),&pCurNode](leaf_node& branch) {
                    this->m_emplaceFunc(branch.m_container, obj);
                    pCurNode = nullptr;
                }
            }, *pCurNode);
            nAxis = (nAxis + 1) % 3;
        }
    }


    template<typename LeafNodeContainerType, typename ValueType>
    typename KdTree<LeafNodeContainerType, ValueType>::node_t* KdTree<LeafNodeContainerType, ValueType>::subdivide(std::vector<glm::vec3> vPoint3, int nAxis, int nLevel) {
        if (vPoint3.empty()) {
            m_vNodes.emplace_back(leaf_node{});
            return &m_vNodes.back();
        }
        std::ranges::sort(vPoint3, [nAxis](const glm::vec3& a, const glm::vec3& b) mutable { 
           return a[nAxis] < b[nAxis];
        });
#ifdef _LOG
        std::cout << "Input:\n";
        for (const auto& p : vPoint3) {
            std::cout << "\t" << glm::to_string(p) << "\n";
        }
        std::cout << "--------\n";
#endif
        static const auto getMedian = [](std::vector<glm::vec3>& v) {
            auto median = v.begin() + (v.size() / 2);
            if (v.size() % 2 == 0)
                --median;
            return median;
        };
        auto median = getMedian(vPoint3);

#ifdef _LOG
        std::cout << "Median: " << glm::to_string(*median) << ", depth: " << nLevel << "\n";
#endif        

        m_vNodes.emplace_back(branch_node{ .m_fLocation = (*median)[nAxis] });
        auto pRet = &m_vNodes.back();
        auto& node = std::get<branch_node>(*pRet);

        if (node.m_fLocation > m_outerBox.maxVertex()[nAxis])
            m_outerBox.maxVertex()[nAxis] = node.m_fLocation;
        if (node.m_fLocation < m_outerBox.minVertex()[nAxis])
            m_outerBox.minVertex()[nAxis] = node.m_fLocation;

        vPoint3.erase(median);
        median = getMedian(vPoint3);
        
        if (vPoint3.size() > 1 || (vPoint3.size() == 1 && vPoint3.front()[nAxis] > node.m_fLocation))
        {
            std::vector<glm::vec3> vRightOfMedian( std::make_move_iterator(median + (vPoint3.size() == 1 ? 0 : 1)), std::make_move_iterator(vPoint3.end()) );
#ifdef _LOG
            std::cout << "vRightOfMedian = " << vRightOfMedian.size() << "\n";
            for (const auto& p : vRightOfMedian) {
                std::cout << "\t" << glm::to_string(p) << "\n";
            }
            std::cout << "--------\n";
#endif
            node.m_pRight = subdivide(std::move(vRightOfMedian), (nAxis + 1) % 3, nLevel + 1);
        } else {
            m_vNodes.emplace_back(leaf_node{});
            node.m_pRight = &m_vNodes.back();
        }
        if (vPoint3.size() > 1 || (vPoint3.size() == 1 && vPoint3.front()[nAxis] <= node.m_fLocation))
        {
            std::vector<glm::vec3> vLeftOfMedian( std::make_move_iterator(vPoint3.begin()), std::make_move_iterator(median + 1) );
#ifdef _LOG
            std::cout << "vLeftOfMedian = " << vLeftOfMedian.size() << "\n";
            for (const auto& p : vLeftOfMedian) {
                std::cout << "\t" << glm::to_string(p) << "\n";
            }
            std::cout << "--------\n";
#endif
            node.m_pLeft = subdivide(std::move(vLeftOfMedian), (nAxis + 1) % 3, nLevel + 1);
        } else {
            m_vNodes.emplace_back(leaf_node{});
            node.m_pLeft = &m_vNodes.back();
        }
        return pRet;
    }
    
    template<typename LeafNodeContainerType, typename ValueType>
    KdTree<LeafNodeContainerType, ValueType>::KdTree(std::vector<glm::vec3> vPoints) {
        construct(std::move(vPoints));
    } 

    template<typename LeafNodeContainerType, typename ValueType>
    void KdTree<LeafNodeContainerType, ValueType>::construct(std::vector<glm::vec3> vPoints) {
        if (!m_vNodes.empty())
            m_vNodes.clear();
        m_vNodes.reserve(2 * vPoints.size() + 1);
        m_pRoot = subdivide(vPoints, 0);
#ifdef _LOG
        std::cout << "Size of container: " << m_vNodes.size() << ", capacity: " << m_vNodes.capacity() << "\n";
#endif
     }

    template<typename LeafNodeContainerType, typename ValueType>
    void KdTree<LeafNodeContainerType, ValueType>::print(typename KdTree<LeafNodeContainerType, ValueType>::node_t* pNode, int nAxis) {
        if (pNode == nullptr)
            pNode = m_pRoot;
        
        std::visit(visitor{ 
            [this, nAxis](const branch_node& node) { 
                std::cout << "(" << node.m_fLocation << ", " << nAxis << ", ";
                if (node.m_pLeft) {
                    std::cout << "<--- ";
                    print(node.m_pLeft, (nAxis + 1) % 3);
                }
                else 
                    std::cout << "()";
                std::cout << ", ";
                if (node.m_pRight) {
                    std::cout << "---> ";
                    print(node.m_pRight, (nAxis + 1) % 3);
                }
                else 
                    std::cout << "()";
                std::cout << ")";
                std::cout << "\n";
            },
            [this](const leaf_node& node) {
                std::cout << "Leaf node: " << node.m_vObjects.size() << " elements\n";
            }
        }, *pNode);
    }

    template<typename LeafNodeContainerType, typename ValueType>
    void KdTree<LeafNodeContainerType, ValueType>::getVisibleObjects(const Frustum* pFrustum) const {
        getVisibleObjects_impl(pFrustum, m_outerBox, nullptr, 0);
    }

    template<typename LeafNodeContainerType, typename ValueType>
    void KdTree<LeafNodeContainerType, ValueType>::getVisibleObjects_impl(const Frustum* pFrustum, 
        const AABB& box,
        node_t* pCurNode,
        int nAxis
    ) const {
        if (pCurNode == nullptr)
            pCurNode = m_pRoot;
        std::visit(visitor{ 
            [&box,pFrustum,this,nAxis,pCurNode](const branch_node& branch) mutable {
                static const auto traverse = [](const KdTree* pThis, const branch_node& branch, const Frustum* pFrustum, AABB nextBox, int nAxis, /*std::vector<T>& vRet, std::unordered_map<ModelType, std::vector<ModelInstanceType*>>& mOut,*/ node_t* pCurNode, bool bIsLeft) {
                    const auto pNextNode = bIsLeft ? branch.m_pLeft : branch.m_pRight;
                    auto& pointToModify = bIsLeft ? nextBox.maxVertex() : nextBox.minVertex();
                    pointToModify[nAxis] = branch.m_fLocation;
                    if (const auto intersectRes = pFrustum->intersects(nextBox, true); intersectRes != FrustumAABBIntersectionType::NO_INTERSECTION) {
                        if (intersectRes == FrustumAABBIntersectionType::CONTAINED) {
                            std::visit([pThis](auto&& next_node) {
                                pThis->m_getVisibleObjectCollectionFunc(next_node.m_container);
                            }, *pNextNode);
                        } else {
                            pThis->getVisibleObjects_impl(pFrustum, nextBox, pNextNode, (nAxis + 1) % 3);
                        }
                    }
            };
            traverse(this, branch, pFrustum, box, nAxis, pCurNode, true);
            traverse(this, branch, pFrustum, box, nAxis, pCurNode, false);
        },
            [this](const leaf_node& branch) {
                this->m_getVisibleObjectCollectionFunc(branch.m_container);
            }
        }, *pCurNode);
    }

    template<typename LeafNodeContainerType, typename ValueType>
    bool KdTree<LeafNodeContainerType, ValueType>::intersects(const Ray& r, node_t* pCurNode, const AABB& box, int nAxis) const {
        bool bRet{};
        if (!pCurNode)
            return false;
        std::visit(visitor{ 
            [&](const branch_node& branch) {
                if (branch.m_pLeft) {
                    auto boxLeft = box;
                    boxLeft.maxVertex()[nAxis] = branch.m_fLocation;
                    if (boxLeft.intersects(r)) {
                        intersects(r, branch.m_pLeft, boxLeft, (nAxis + 1) % 3);
                    }
                }
                if (branch.m_pRight) {
                    auto boxRight = box;
                    boxRight.minVertex()[nAxis] = branch.m_fLocation;
                    if (boxRight.intersects(r)) {
                        intersects(r, branch.m_pRight, boxRight, (nAxis + 1) % 3);
                    }
                }
             },
            [this,&bRet,&r](const leaf_node& leaf) {
                bRet = this->m_intersectsCollectionFunc(r, leaf.m_container);
            }
        }, *pCurNode);
        return bRet;
    }

    template<typename LeafNodeContainerType, typename ValueType>
    bool KdTree<LeafNodeContainerType, ValueType>::intersects(const Ray& r) const {
        return intersects(r, m_pRoot, m_outerBox, 0);
    }

} // namespace Ice
#endif