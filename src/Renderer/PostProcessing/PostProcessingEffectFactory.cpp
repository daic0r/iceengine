#include <Renderer/PostProcessing/PostProcessingEffectFactory.h>

#ifdef RENDERER_OPEN_GL
#include <Renderer/PostProcessing/OpenGL/OriginalCanvasGL.h>
#include <Renderer/PostProcessing/OpenGL/ContrastChangerGL.h>
#include <Renderer/PostProcessing/OpenGL/GaussianBlurGL.h>
#endif

namespace Ice::PostProcessingEffectFactory
{
     
     std::unique_ptr<IPostProcessingEffect> create(PostProcessingEffect effect, int nWidth, int nHeight)
     {
        switch (effect) {
            case PostProcessingEffect::ORIGINAL_CANVAS:
#ifdef RENDERER_OPEN_GL
                {
                    auto pRet = std::make_unique<OriginalCanvasGL>(nWidth, nHeight);
                    pRet->initialize();
                    return pRet;
                }
#endif
                break;
            case PostProcessingEffect::CONTRAST_CHANGER:
#ifdef RENDERER_OPEN_GL
                {
                    auto pRet = std::make_unique<ContrastChangerGL>(nWidth, nHeight);
                    pRet->initialize();
                    return pRet;
                }
#endif
                break;
            case PostProcessingEffect::HORIZONTAL_GAUSSIAN_BLUR:
#ifdef RENDERER_OPEN_GL
                {
                    auto pRet = std::make_unique<HorizontalGaussianBlurGL>(nWidth, nHeight);
                    pRet->initialize();
                    return pRet;
                }
#endif
                break;
            case PostProcessingEffect::VERTICAL_GAUSSIAN_BLUR:
#ifdef RENDERER_OPEN_GL
                {
                    auto pRet = std::make_unique<VerticalGaussianBlurGL>(nWidth, nHeight);
                    pRet->initialize();
                    return pRet;
                }
#endif
                break;
          }
        return nullptr;
    }

} // namespace Ice::PostProcessingEffectFactory
