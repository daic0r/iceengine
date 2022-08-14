#ifndef KdTree_h
#define KdTree_h

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
            glm::vec3 m_point; // #TODO: save only the float that defines the axis
            splitting_axis m_axis;
            node_t* m_pLeft{}, *m_pRight{};
        };
        struct leaf_node {
            std::vector<T> m_vObjects;
        };
        std::vector<node_t> m_vNodes{};

        node_t* _subdivide(std::vector<glm::vec3>, int nAxis, int nLevel = 0);
    public:
        KdTree() = default;
        KdTree(const std::vector<float>& vPoints);
        void construct(const std::vector<float>& vPoints);
        void print(node_t* pNode = nullptr);

        template<typename Visitor> requires (BranchTraversingVisitor<Visitor> && LeafVisitor<Visitor, T>)
        void traverse(Visitor&& v) const {
            auto pCurNode = m_pRoot;
            AABB boxLeft{
                glm::vec3{ -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() },
                glm::vec3{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() },
            }, boxRight = boxLeft;
            bool bAbort{};
            while (!bAbort) {
                std::visit(visitor{ 
                    [&pCurNode,&bAbort,&boxLeft,&boxRight,v=std::forward<Visitor>(v)](const branch_node& branch) mutable {
                        const auto nAxis = static_cast<int>(branch.m_axis);
                        boxLeft.maxVertex()[nAxis] = branch.m_point[nAxis];
                        boxRight.minVertex()[nAxis] = branch.m_point[nAxis];
                        const auto ret = std::forward<Visitor>(v)(boxLeft, boxRight);
                        switch (ret) {
                            case TreeBranch::NONE:
                                bAbort = true;
                                break;
                            case TreeBranch::LEFT:
                                pCurNode = branch.m_pLeft;
                                if (!pCurNode)
                                    bAbort = true;
                                break;
                            case TreeBranch::RIGHT:
                                pCurNode = branch.m_pRight;
                                if (!pCurNode)
                                    bAbort = true;
                                break;
                        } 
                    },
                    [&bAbort,v=std::forward<Visitor>(v)](const leaf_node& branch) mutable {
                        std::forward<Visitor>(v)(branch.m_vObjects);
                        bAbort = true;
                    }
                }, *pCurNode);
            }
        }

    private:
        node_t* m_pRoot{};
    };
} // namespace Ice

#endif