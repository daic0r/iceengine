#ifndef IContrastChanger_h
#define IContrastChanger_h

namespace Ice
{
    class IContrastChanger {
    public:
        virtual void setContrast(float) noexcept = 0;
        virtual ~IContrastChanger() = default;
    };
} // namespace Ice


#endif