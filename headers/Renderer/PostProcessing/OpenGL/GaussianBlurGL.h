#ifndef HorizontalGaussianBlurGL_h
#define HorizontalGaussianBlurGL_h

#ifdef RENDERER_OPEN_GL

#include <Renderer/PostProcessing/OpenGL/BasePostProcessorGL.h>
#include <Interfaces/PostProcessing/IGaussianBlur.h>

namespace Ice
{
    class OneDimensionalGaussianBlurGL : public BasePostProcessorGL {
    public:
        using BasePostProcessorGL::BasePostProcessorGL;

        void initialize() override;
        void setKernelSize(std::size_t n) noexcept;
        void resize(int nWidth, int nHeight) override;
    private:
        virtual void setTexelDimension() noexcept = 0;

    protected:
        std::size_t m_nKernelSize{ 11 };
        GLint m_nTexelDimensionUniformId{ -1 };
        GLint m_nKernelSizeUniformId{ -1 };
     };

    class HorizontalGaussianBlurGL : public OneDimensionalGaussianBlurGL, public IGaussianBlur {
    public:
        using OneDimensionalGaussianBlurGL::OneDimensionalGaussianBlurGL;

        void setKernelSize(std::size_t n) noexcept override;
    private:
        void setTexelDimension() noexcept override;
        const char* getFragmentShaderSource() const noexcept override;
        
    };

    class VerticalGaussianBlurGL : public OneDimensionalGaussianBlurGL, public IGaussianBlur {
    public:
        using OneDimensionalGaussianBlurGL::OneDimensionalGaussianBlurGL;

        void setKernelSize(std::size_t n) noexcept override;
    private:
        void setTexelDimension() noexcept override; 
        const char* getFragmentShaderSource() const noexcept override;
        
    };

/*
    class GaussianBlurGL : public IGaussianBlur, public IPostProcessingEffect {
    public:
        GaussianBlurGL(GLsizei nWidth = -1, GLsizei nHeight = -1);

        void bind();
        void postProcess(IPostProcessingEffect* pPrevious);
        void unbind();
        void resize(int nWidth, int nHeight);
        bool hasStaticSize() const noexcept;

        void initialize();
        void setKernelSize(std::size_t n) noexcept override;
private:
        HorizontalGaussianBlurGL m_horBlur;
        VerticalGaussianBlurGL m_verBlur;
    };
*/
} // namespace Ice

#endif

#endif