#ifndef KdTree_h
#define KdTree_h

#include <unordered_set>
#include <vector>
#include <memory>
#include <glm/vec3.hpp>
#include <variant>
#include <type_traits>
#include <System/AABB.h>
#include <limits>
#include <Utils/TemplateHelpers.h>

namespace Ice
{
    class Frustum;

    template<typename T>
    class KdTree {
        struct leaf_node;
        struct branch_node;
        using node_t = std::variant<branch_node, leaf_node>;
        struct branch_node {
            float m_fLocation{}; 
            node_t* m_pLeft{}, *m_pRight{};
        };
        struct leaf_node {
            std::vector<T> m_vObjects;
        };

        node_t* subdivide(std::vector<glm::vec3>, int nAxis, int nLevel = 0);
        void getVisibleObjects_impl(const Frustum*, const AABB& box, std::vector<T>& vRet, node_t* pCurNode = nullptr, int nAxis = 0) const;

    public:
        KdTree() = default;
        KdTree(std::vector<glm::vec3> vPoints);
        void construct(std::vector<glm::vec3> vPoints);
        void print(node_t* pNode = nullptr, int nAxis = 0);
        template<typename U = T>
        void emplace(const glm::vec3& p, U&& u) {
            auto pCurNode = m_pRoot;
            int nAxis{};
            while (pCurNode) {
                std::visit(visitor{ 
                    [&p,&pCurNode,nAxis](const branch_node& branch) {
                        pCurNode = p[nAxis] <= branch.m_fLocation ? branch.m_pLeft : branch.m_pRight;
                    },
                    [obj=std::forward<U>(u),&pCurNode](leaf_node& branch) {
                        if (std::ranges::none_of(branch.m_vObjects, [obj=std::forward<U>(obj)](auto&& v) { return v == obj; }))
                            branch.m_vObjects.emplace_back(std::forward<U>(obj));
                        pCurNode = nullptr;
                    }
                }, *pCurNode);
                nAxis = (nAxis + 1) % 3;
            }
         }

        void getVisibleObjects(const Frustum*, std::vector<T>&) const;
   private:
        node_t* m_pRoot{};
        std::vector<node_t> m_vNodes{};
        AABB m_outerBox{};
    };
} // namespace Ice

#endif