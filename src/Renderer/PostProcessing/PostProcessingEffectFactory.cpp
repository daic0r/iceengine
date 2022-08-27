#include <Renderer/PostProcessing/PostProcessingEffectFactory.h>

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
        }
        return nullptr;
    }

} // namespace Ice::PostProcessingEffectFactory
