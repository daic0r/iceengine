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

    enum class TreeBranch {
        NONE,
        LEFT,
        RIGHT
    };

    template<typename Callable>
    concept BranchTraversingVisitor = requires {
        // requires keyword checks that the expression following it is true
        // otherwise, only the well-formedness of the expression would be checked
        requires std::is_same_v<std::invoke_result_t<Callable, AABB, AABB>, TreeBranch>;
    };
    template<typename Callable, typename ValueType>
    concept LeafVisitor = requires {
        requires std::is_same_v<std::invoke_result_t<Callable, std::vector<ValueType>>, void>;
    };

    template<typename T>
    class KdTree {
        enum class splitting_axis {
            X,
            Y,
            Z
        };
        struct leaf_node;
        struct branch_node;
        using node_t = std::variant<branch_node, leaf_node>;
        struct branch_node {
            float m_fLocation{}; 
            splitting_axis m_axis;
            node_t* m_pLeft{}, *m_pRight{};
        };
        struct leaf_node {
            std::vector<T> m_vObjects;
        };

        node_t* _subdivide(std::vector<glm::vec3>, int nAxis, int nLevel = 0);
        void getVisibleObjects_impl(const Frustum*, AABB box, std::vector<T>& vRet, node_t* pCurNode = nullptr, int nAxis = 0) const;

    public:
        KdTree() = default;
        KdTree(const std::vector<float>& vPoints);
        void construct(const std::vector<float>& vPoints);
        void print(node_t* pNode = nullptr, int nAxis = 0);
        template<typename U = T>
        void emplace(const glm::vec3& p, U&& u) {
            auto pCurNode = m_pRoot;
            //int nAxis{};
            while (pCurNode) {
                std::visit(visitor{ 
                    [&p,&pCurNode](const branch_node& branch) {
                        const auto nAxis = static_cast<int>(branch.m_axis);
                        pCurNode = p[nAxis] <= branch.m_fLocation ? branch.m_pLeft : branch.m_pRight;
                    },
                    [obj=std::forward<U>(u),&pCurNode](leaf_node& branch) {
                        if (std::ranges::none_of(branch.m_vObjects, [obj=std::forward<U>(obj)](auto&& v) { return v == obj; }))
                            branch.m_vObjects.emplace_back(std::forward<U>(obj));
                        pCurNode = nullptr;
                    }
                }, *pCurNode);
                //nAxis = (nAxis + 1) % 3;
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