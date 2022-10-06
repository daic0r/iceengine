#ifdef _DEBUG_OCTREE
#ifdef RENDERER_OPEN_GL

#include <GL/glad.h>
#include <Renderer/OpenGL/OctreeDebuggerGL.h>
#include <array>
#include <glm/vec3.hpp>
#include <Renderer/OpenGL/VertexAttributeGL.h>
#include <System/Octree.h>
#include <Renderer/RenderEnvironment.h>

namespace Ice
{
    static std::vector<glm::vec3> CubeVertices {
        glm::vec3{ -0.5f, -0.5f, -0.5f },
        glm::vec3{ -0.5f, -0.5f, 0.5f },
        glm::vec3{ 0.5f, -0.5f, 0.5f },
        glm::vec3{ 0.5f, -0.5f, -0.5f },
        glm::vec3{ -0.5f, 0.5f, -0.5f },
        glm::vec3{ -0.5f, 0.5f, 0.5f },
        glm::vec3{ 0.5f, 0.5f, 0.5f },
        glm::vec3{ 0.5f, 0.5f, -0.5f },
    };

    static const std::vector<unsigned int> Indices {
        // Front
        7, 3, 0,
        0, 4, 7,
        // Back
        5, 1, 2,
        2, 6, 5,
        // Left
        6, 2, 3,
        3, 7, 6,
        // Right
        4, 0, 1,
        1, 5, 4,
        // Bottom
        1, 0, 3,
        3, 2, 1,
        // Top
        6, 7, 4,
        4, 5, 6
    };

    OctreeDebuggerGL::OctreeDebuggerGL() {
        m_vao.create();
        m_vao.bind();
        m_vao.addStaticVertexAttribute<glm::vec3>(0);
        m_vao.attribute(0).setData(&CubeVertices);
        m_vao.attribute(0).connect();
        m_vao.setIndexBuffer(Indices);

        m_shader.fromSource(R"(
            #version 410

            layout(location = 0) in vec3 vertexPos;

            uniform mat4 projViewMatrix;
            uniform vec3 pos;
            uniform vec3 size;

            void main() {
                vec4 worldPos = projViewMatrix * mat4(
                    size.x, 0, 0, 0,
                    0, size.y, 0, 0,
                    0, 0, size.z, 0,
                    pos.xyz, 1) * vec4(vertexPos, 1);
                gl_Position = worldPos;
            }
        )",
        R"(
            #version 410

            out vec4 outColor;

            uniform bool highlight;

            void main() {
                if (highlight)
                    outColor = vec4(1,0,0,1);
                else
                    outColor = vec4(1,1,0,1);
            }
        )");
        m_nPosID = m_shader.getUniformLocation("pos");
        m_nSizeID = m_shader.getUniformLocation("size");
        m_nHighlightID = m_shader.getUniformLocation("highlight");
        m_nProjViewMatrixID = m_shader.getUniformLocation("projViewMatrix");
    }

    void OctreeDebuggerGL::render(const RenderEnvironment& env, IOctreeTraversal* pOctree) {
        m_shader.use();
        m_shader.loadMatrix4f(m_nProjViewMatrixID, env.projectionMatrix * env.viewMatrix);
        m_vao.bind();
        m_vao.bindIndexVBO();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (auto info = pOctree->startTraversal(); info.box != nullptr; info = pOctree->next()) {
            const auto& [nID, pBB, bIsLeaf] = info;
            if (bIsLeaf) {
                const auto cen = pBB->center();
                const auto siz = glm::vec3{ pBB->width(), pBB->height(), pBB->depth() };
                m_shader.loadVector3f(m_nPosID, cen);
                m_shader.loadVector3f(m_nSizeID, siz);
                m_shader.loadInt(m_nHighlightID, std::ranges::any_of(pOctree->intersected(), [nID](const auto elem) { return elem == nID; }));
                glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, nullptr);
            }
        }
        m_shader.unuse();
        m_vao.unbind();
    }
    
} // namespace Ice

#endif
#endif