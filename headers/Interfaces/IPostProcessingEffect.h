#ifndef IPostProcessingEffect_h
#define IPostProcessingEffect_h

namespace Ice
{
    class IPostProcessingEffect {
    public:
        virtual void bind() = 0;
        virtual void postProcess(IPostProcessingEffect* pPrevious) = 0;
        virtual void unbind() = 0;
        virtual void resize(int nWidth, int nHeight) = 0;
        virtual bool hasStaticSize() const noexcept = 0;
        virtual ~IPostProcessingEffect() = default;
    };
    
} // namespace Ice


#endif