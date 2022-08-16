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

    namespace detail
    {
        template <std::ranges::range R>
        auto to_vector(R&& r) {
            std::vector<std::ranges::range_value_t<R>> v;

            // if we can get a size, reserve that much
            if constexpr (requires { std::ranges::size(r); }) {
                v.reserve(std::ranges::size(r));
            }

            // push all the elements
            for (auto&& e : r) {
                v.push_back(static_cast<decltype(e)&&>(e));
            }

            return v;
        } 
    } // namespace detail

    template<typename T>
    typename KdTree<T>::node_t* KdTree<T>::_subdivide(std::vector<glm::vec3> vPoint3, int nAxis, int nLevel) {
        std::ranges::sort(vPoint3, [nAxis](const glm::vec3& a, const glm::vec3& b) mutable { 
            //std::cout << nAxis << "\n";
            /*
            for (int i = 0; i < 3; ++i, nAxis = (nAxis + 1) % 3) {
                if (!Ice::Math::equal(a[nAxis], b[nAxis]))
                    break;
            }
            */
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

        m_vNodes.emplace_back(branch_node{ *median, static_cast<splitting_axis>(nAxis) });
        auto pRet = &m_vNodes.back();
        auto& node = std::get<branch_node>(*pRet);

        if (node.m_point[nAxis] > m_outerBox.maxVertex()[nAxis])
            m_outerBox.maxVertex()[nAxis] = node.m_point[nAxis];
        if (node.m_point[nAxis] < m_outerBox.minVertex()[nAxis])
            m_outerBox.minVertex()[nAxis] = node.m_point[nAxis];

        vPoint3.erase(median);
        median = getMedian(vPoint3);
        
        if (vPoint3.size() > 1 || (vPoint3.size() == 1 && vPoint3.front()[nAxis] > node.m_point[nAxis]))
        {
            std::vector<glm::vec3> vRightOfMedian( std::make_move_iterator(median + (vPoint3.size() == 1 ? 0 : 1)), std::make_move_iterator(vPoint3.end()) );
#ifdef _LOG
            std::cout << "vRightOfMedian = " << vRightOfMedian.size() << "\n";
            for (const auto& p : vRightOfMedian) {
                std::cout << "\t" << glm::to_string(p) << "\n";
            }
            std::cout << "--------\n";
#endif
            node.m_pRight = _subdivide(std::move(vRightOfMedian), (nAxis + 1) % 3, nLevel + 1);
        } else {
            m_vNodes.emplace_back(leaf_node{});
            node.m_pRight = &m_vNodes.back();
        }
        if (vPoint3.size() > 1 || (vPoint3.size() == 1 && vPoint3.front()[nAxis] <= node.m_point[nAxis]))
        {
            std::vector<glm::vec3> vLeftOfMedian( std::make_move_iterator(vPoint3.begin()), std::make_move_iterator(median + 1) );
#ifdef _LOG
            std::cout << "vLeftOfMedian = " << vLeftOfMedian.size() << "\n";
            for (const auto& p : vLeftOfMedian) {
                std::cout << "\t" << glm::to_string(p) << "\n";
            }
            std::cout << "--------\n";
#endif
            node.m_pLeft = _subdivide(std::move(vLeftOfMedian), (nAxis + 1) % 3, nLevel + 1);
        } else {
            m_vNodes.emplace_back(leaf_node{});
            node.m_pLeft = &m_vNodes.back();
        }
        return pRet;
    }
    
    template<typename T>
    KdTree<T>::KdTree(const std::vector<float>& vPoints) {
        construct(vPoints);
    } 

    template<typename T>
    void KdTree<T>::construct(const std::vector<float>& vPoints) {
        //m_vPoints = vPoints;

        std::vector<glm::vec3> vPoint3;
        vPoint3.reserve(vPoints.size() / 3);
        for (std::size_t i{}; i < vPoints.size(); i+=3)
            vPoint3.emplace_back(vPoints[i], vPoints[i+1], vPoints[i+2]);

        m_vNodes.reserve(2 * vPoint3.size() + 1);
        m_pRoot = _subdivide(vPoint3, 0);
#ifdef _LOG
        std::cout << "Size of container: " << m_vNodes.size() << ", capacity: " << m_vNodes.capacity() << "\n";
#endif
     }

    template<typename T>
    void KdTree<T>::print(typename KdTree<T>::node_t* pNode) {
        if (pNode == nullptr)
            pNode = m_pRoot;
        
        std::visit(visitor{ 
            [this](const branch_node& node) { 
                std::cout << "(" << glm::to_string(node.m_point) << ", " << static_cast<int>(node.m_axis) << ", ";
                if (node.m_pLeft) {
                    std::cout << "<--- ";
                    print(node.m_pLeft);
                }
                else 
                    std::cout << "()";
                std::cout << ", ";
                if (node.m_pRight) {
                    std::cout << "---> ";
                    print(node.m_pRight);
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
    std::unordered_set<T> KdTree<T>::getVisibleObjects(const Frustum* pFrustum) const {
        return getVisibleObjects_impl(pFrustum, m_outerBox, nullptr);
    }

    template<typename T>
    std::unordered_set<T> KdTree<T>::getVisibleObjects_impl(const Frustum* pFrustum, 
        AABB box, 
        node_t* pCurNode
    ) const {
        std::unordered_set<T> vRet{};
        if (pCurNode == nullptr)
            pCurNode = m_pRoot;
        std::visit(visitor{ 
            [&box,&vRet,pFrustum,this](const branch_node& branch) {
                const auto nAxis = static_cast<int>(branch.m_axis);
                if (!pFrustum) {
                    if (branch.m_pLeft) {
                        auto vTemp = getVisibleObjects_impl(pFrustum, box, branch.m_pLeft);
                        std::move(vTemp.begin(), vTemp.end(), std::inserter(vRet, vRet.end()));
                    }
                    if (branch.m_pRight) {
                        auto vTemp = getVisibleObjects_impl(pFrustum, box, branch.m_pRight);
                        std::move(vTemp.begin(), vTemp.end(), std::inserter(vRet, vRet.end()));
                    }
                } else {
                    if (branch.m_pLeft) {
                        AABB boxLeft = box;
                        boxLeft.maxVertex()[nAxis] = branch.m_point[nAxis];
                        if (const auto intersectRes = pFrustum->intersects(boxLeft, true); intersectRes != FrustumAABBIntersectionType::NO_INTERSECTION) {
                            auto pPassFrustum = intersectRes == FrustumAABBIntersectionType::CONTAINED ? nullptr : pFrustum;
                            auto vTemp = getVisibleObjects_impl(pPassFrustum, boxLeft, branch.m_pLeft);
                            std::move(vTemp.begin(), vTemp.end(), std::inserter(vRet, vRet.end()));
                        }
                    }
                    if (branch.m_pRight) {
                        AABB boxRight = box;
                        boxRight.minVertex()[nAxis] = branch.m_point[nAxis];
                        if (const auto intersectRes = pFrustum->intersects(boxRight, true); intersectRes != FrustumAABBIntersectionType::NO_INTERSECTION) {
                            auto pPassFrustum = intersectRes == FrustumAABBIntersectionType::CONTAINED ? nullptr : pFrustum;
                            auto vTemp = getVisibleObjects_impl(pPassFrustum, boxRight, branch.m_pRight);
                            std::move(vTemp.begin(), vTemp.end(), std::inserter(vRet, vRet.end()));
                        }
                    }
                }
            },
            [&vRet](const leaf_node& branch) {
                vRet.insert(branch.m_vObjects.begin(), branch.m_vObjects.end());
            }
        }, *pCurNode);

        return vRet;
    }

    template class KdTree<Entity>;
} // namespace Ice
