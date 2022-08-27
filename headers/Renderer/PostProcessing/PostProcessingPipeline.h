#ifndef PostProcessingPipleline_h
#define PostProcessingPipleline_h

#include <Interfaces/IPostProcessingEffect.h>
#include <memory>
#include <vector>
#include <tuple>

namespace Ice
{
    class PostProcessingPipeline {
    public:
        IPostProcessingEffect* originalCanvas() const;
        void add(std::size_t nOrder, std::unique_ptr<IPostProcessingEffect> pEffect);
        void run();
        void resize(int nWidth, int nHeight);

    private:
        mutable std::unique_ptr<IPostProcessingEffect> m_pOriginalCanvas{};
        std::vector<std::pair<std::size_t, std::unique_ptr<IPostProcessingEffect>>> m_vEffects;
    };
} // namespace Ice


#endif