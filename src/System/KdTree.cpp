#include <System/KdTree.h>
#include <Entities/Entity.h>
#include <ranges>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

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
    typename KdTree<T>::node* KdTree<T>::_subdivide(std::vector<glm::vec3> vPoint3, int nAxis) {
        if (vPoint3.size() < 3)
            return nullptr;

        std::ranges::sort(vPoint3, [nAxis](const glm::vec3& a, const glm::vec3& b) { return a[nAxis] < b[nAxis]; });

        auto median = vPoint3.begin() + (vPoint3.size() / 2);
        m_vNodes.emplace_back(*median, static_cast<splitting_axis>(nAxis));
        auto pRet = &m_vNodes.back();
        
        std::vector<glm::vec3> vLeftOfMedian( vPoint3.begin(), median + 1 );
        std::vector<glm::vec3> vRightOfMedian( median + 1, vPoint3.end() );

        pRet->m_pLeft = _subdivide(std::move(vLeftOfMedian), (nAxis + 1) % 3);
        pRet->m_pRight = _subdivide(std::move(vRightOfMedian), (nAxis + 1) % 3);

        return pRet;
    }
    
    template<typename T>
    KdTree<T>::KdTree(const std::vector<float>& vPoints) {
        //m_vPoints = vPoints;

        std::vector<glm::vec3> vPoint3;
        vPoint3.reserve(vPoints.size() / 3);
        for (std::size_t i{}; i < vPoints.size(); i+=3)
            vPoint3.emplace_back(vPoints[i], vPoints[i+1], vPoints[i+2]);
        
        m_pRoot = _subdivide(vPoint3, 0);

    } 

    template<typename T>
    void KdTree<T>::print(typename KdTree<T>::node* pNode) {
        if (pNode == nullptr)
            pNode = m_pRoot;
        
        std::cout << "(" << glm::to_string(pNode->m_point) << ", " << static_cast<int>(pNode->m_axis) << ", ";
        if (pNode->m_pLeft)
            print(pNode->m_pLeft);
        else 
            std::cout << "()";
        std::cout << ", ";
        if (pNode->m_pRight)
            print(pNode->m_pRight);
        else 
            std::cout << "()";
        std::cout << ")";
        std::cout << "\n";
    }

    template class KdTree<Entity>;
} // namespace Ice
