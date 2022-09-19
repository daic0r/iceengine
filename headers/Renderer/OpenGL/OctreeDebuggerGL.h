#ifndef OctreeDebuggerGL_h
#define OctreeDebuggerGL_h

#ifdef _DEBUG_OCTREE
#ifdef RENDERER_OPEN_GL

#include <Interfaces/Renderer/IOctreeDebugger.h>
#include <Renderer/OpenGL/VAO.h>
#include <Renderer/OpenGL/VBO.h>
#include <ShadersGL/ShaderProgramGL.h>

namespace Ice
{
    class OctreeDebuggerGL : public IOctreeDebugger {
        VAO m_vao;
        VBO m_vbo;
        ShaderProgramGL m_shader;
        GLint m_nPosID{-1};
        GLint m_nSizeID{-1};
        GLint m_nHighlightID{-1};
        GLint m_nProjViewMatrixID{-1};
    public:
        OctreeDebuggerGL();
        void render(const RenderEnvironment& env, IOctreeTraversal* pOctree) override;

    };
} // namespace Ice

#endif
#endif
#endif