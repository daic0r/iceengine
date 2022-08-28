#ifndef IGaussianBlur_h
#define IGaussianBlur_h

namespace Ice
{
    class IGaussianBlur {
    public:
        virtual void setKernelSize(std::size_t nKernelSize) = 0;
        virtual ~IGaussianBlur() = default;
    };
} // namespace Ice

#endif
