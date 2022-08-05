#ifndef File_h
#define File_h

#include <string_view>
#include <string>

namespace Ice
{
    class File {
        std::string m_strPath;
    public:
        File(std::string_view);
        operator const std::string&() const;
        operator std::string_view() const;
    };    

    class AssetFile : public File {
    public:
        AssetFile(std::string_view);
    };

    class DataFile : public File {
    public:
        DataFile(std::string_view);
    };
} // namespace Ice

#endif
