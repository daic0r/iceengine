#include <System/File.h>
#include <System/SystemServices.h>
#include <System/Config.h>

namespace Ice
{
    File::File(std::string_view strFile) {
        m_strPath = strFile; // systemServices.getConfig()->prependAssetPath(strFile);
    }

    File::operator const std::string&() const {
        return m_strPath;
    }

    File::operator std::string_view() const {
        return m_strPath;
    }


    AssetFile::AssetFile(std::string_view strFile)
        : File{ systemServices.getConfig()->prependAssetPath(strFile) }
    {
    }


    DataFile::DataFile(std::string_view strFile)
        : File{ systemServices.getConfig()->prependDataPath(strFile) }
    {
    }
} // namespace Ice
