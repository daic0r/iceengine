#include <Renderer/PostProcessing/PostProcessingPipeline.h>
#include <Renderer/PostProcessing/PostProcessingEffectFactory.h>
#include <algorithm>

namespace Ice {

    IPostProcessingEffect* PostProcessingPipeline::originalCanvas() const {
        if (!m_pOriginalCanvas) [[unlikely]]
            m_pOriginalCanvas = PostProcessingEffectFactory::create(PostProcessingEffect::ORIGINAL_CANVAS);
        return m_pOriginalCanvas.get();
    }

    void PostProcessingPipeline::add(std::size_t nOrder, std::unique_ptr<IPostProcessingEffect> pEffect) {
        auto value = std::make_pair(nOrder, std::move(pEffect));
        const auto iter = std::ranges::upper_bound(m_vEffects, value, [](const auto& val1, const auto& val2) { return val1.first < val2.first; });
        m_vEffects.insert(iter, std::move(value));
    }
    
    void PostProcessingPipeline::run() {
        auto pPrevious = originalCanvas();
        for (const auto& [nOrder, pEffect] : m_vEffects) {
            pEffect->bind();
            pEffect->postProcess(pPrevious);
            pPrevious = pEffect.get();
        }
        originalCanvas()->unbind();
        originalCanvas()->postProcess(pPrevious);
    }

    void PostProcessingPipeline::resize(int nWidth, int nHeight) {
        if (!m_pOriginalCanvas)
            return;
        originalCanvas()->resize(nWidth, nHeight);
        for (const auto& [nOrder, pEffect] : m_vEffects) {
            pEffect->resize(nWidth, nHeight);
        }
    }

}