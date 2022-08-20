#include <System/KdTree.h>
#include <Entities/Entity.h>
#include <ranges>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <System/Math.h>
#include <Utils/TemplateHelpers.h>
#include <Renderer/Frustum.h>

namespace Ice
{
    template<typename T>
    typename KdTree<T>::node_t* KdTree<T>::subdivide(std::vector<glm::vec3> vPoint3, int nAxis, int nLevel) {
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

        m_vNodes.emplace_back(branch_node{ (*median)[nAxis] });
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
    
    template<typename T>
    KdTree<T>::KdTree(std::vector<glm::vec3> vPoints) {
        construct(std::move(vPoints));
    } 

    template<typename T>
    void KdTree<T>::construct(std::vector<glm::vec3> vPoints) {
        if (!m_vNodes.empty())
            m_vNodes.clear();
        m_vNodes.reserve(2 * vPoints.size() + 1);
        m_pRoot = subdivide(vPoints, 0);
#ifdef _LOG
        std::cout << "Size of container: " << m_vNodes.size() << ", capacity: " << m_vNodes.capacity() << "\n";
#endif
     }

    template<typename T>
    void KdTree<T>::print(typename KdTree<T>::node_t* pNode, int nAxis) {
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

    template<typename T>
    void KdTree<T>::getVisibleObjects(const Frustum* pFrustum, std::vector<T>& vRet) const {
        getVisibleObjects_impl(pFrustum, m_outerBox, vRet, nullptr, 0);
    }

    template<typename T>
    void KdTree<T>::getVisibleObjects_impl(const Frustum* pFrustum, 
        const AABB& box,
        std::vector<T>& vRet, 
        node_t* pCurNode,
        int nAxis
    ) const {
        if (pCurNode == nullptr)
            pCurNode = m_pRoot;
        std::visit(visitor{ 
            [&box,&vRet,pFrustum,this,nAxis](const branch_node& branch) {
                static const auto traverse = [](const KdTree* pThis, const branch_node& branch, const Frustum* pFrustum, AABB nextBox, int nAxis, std::vector<T>& vRet, bool bIsLeft) {
                    const auto pNextNode = bIsLeft ? branch.m_pLeft : branch.m_pRight;
                    if (pFrustum) {
                        auto& pointToModify = bIsLeft ? nextBox.maxVertex() : nextBox.minVertex();
                        pointToModify[nAxis] = branch.m_fLocation;
                        if (const auto intersectRes = pFrustum->intersects(nextBox, true); intersectRes != FrustumAABBIntersectionType::NO_INTERSECTION) {
                            const auto pPassFrustum = intersectRes == FrustumAABBIntersectionType::CONTAINED ? nullptr : pFrustum;
                            pThis->getVisibleObjects_impl(pPassFrustum, nextBox, vRet, pNextNode, (nAxis + 1) % 3);
                        }
                    } else {
                        pThis->getVisibleObjects_impl(nullptr, nextBox, vRet, pNextNode, (nAxis + 1) % 3);
                    }
                };
                traverse(this, branch, pFrustum, box, nAxis, vRet, true);
                traverse(this, branch, pFrustum, box, nAxis, vRet, false);
        },
            [&vRet](const leaf_node& branch) {
                vRet.insert(vRet.end(), branch.m_vObjects.begin(), branch.m_vObjects.end());
            }
        }, *pCurNode);
    }

    template class KdTree<Entity>;
} // namespace Ice
