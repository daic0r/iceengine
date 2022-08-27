#ifndef PostProcessingEffectFactory_h
#define PostProcessingEffectFactory_h

#ifdef RENDERER_OPEN_GL
#include <Renderer/PostProcessing/OpenGL/OriginalCanvasGL.h>
#include <Renderer/PostProcessing/OpenGL/ContrastChangerGL.h>
#endif

#include <memory>

enum class PostProcessingEffect {
    NONE,
    ORIGINAL_CANVAS,
    CONTRAST_CHANGER
};

namespace Ice::PostProcessingEffectFactory
{
    std::unique_ptr<IPostProcessingEffect> create(PostProcessingEffect effect, int nWidth = -1, int nHeight = -1);    
} // namespace Ice


#endif