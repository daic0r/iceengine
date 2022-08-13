#ifndef KdTree_h
#define KdTree_h

#include <vector>
#include <memory>
#include <glm/vec3.hpp>
#include <variant>

namespace Ice
{
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
            glm::vec3 m_point;
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
        void print(node_t* pNode = nullptr);

    private:
        node_t* m_pRoot{};
    };
} // namespace Ice

#endif