#ifndef PostProcessingEffectFactory_h
#define PostProcessingEffectFactory_h

#include <memory>

enum class PostProcessingEffect {
    NONE,
    ORIGINAL_CANVAS,
    CONTRAST_CHANGER,
    HORIZONTAL_GAUSSIAN_BLUR,
    VERTICAL_GAUSSIAN_BLUR,
};

namespace Ice {
    class IPostProcessingEffect;

    namespace PostProcessingEffectFactory {
        std::unique_ptr<IPostProcessingEffect> create(PostProcessingEffect effect, int nWidth = -1, int nHeight = -1);    
    }
} // namespace Ice


#endif