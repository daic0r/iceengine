#ifndef OctreeDebugger_h
#define OctreeDebugger_h

namespace Ice
{
    class IOctreeTraversal;
    struct RenderEnvironment;

    class IOctreeDebugger {
    public:
        virtual void render(const RenderEnvironment&, IOctreeTraversal*) = 0;
        virtual ~IOctreeDebugger() = default;

    };
} // namespace Ice


#endif