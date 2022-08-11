#ifndef KdTree_h
#define KdTree_h

#include <vector>
#include <memory>
#include <glm/vec3.hpp>

namespace Ice
{
    template<typename T>
    class KdTree {
        enum class splitting_axis {
            X,
            Y,
            Z
        };
        struct node {
            glm::vec3 m_point;
            splitting_axis m_axis;
            node* m_pLeft{}, *m_pRight{};
        };
        std::vector<node> m_vNodes{};

        node* _subdivide(std::vector<glm::vec3>, int nAxis);
    public:
        KdTree() = default;
        KdTree(const std::vector<float>& vPoints);
        void print(node* pNode = nullptr);

    private:
        node* m_pRoot{};
        std::vector<float> m_vPoints;
    };
} // namespace Ice

#endif