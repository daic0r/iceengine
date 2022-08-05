//
//  Config.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Config.h>
#include <filesystem>
#include <IceEngine.h>

namespace Ice {

Config::Config() {
    std::filesystem::path p = std::filesystem::current_path();
#ifdef __APPLE__
    p /= "../..";
#elif __linux__
    p /= "..";
#endif
    m_strBasePath = p.string();
    //p.append("Assets/");
    //m_strAssetPath = p.string();
}

Config::Config(int width, int height, bool fullscreen, int windowX/* = -1*/, int windowY/* = -1*/, 
    std::string_view assetPath/* = "Assets"*/, PathType assetPathType/* = PathType::RELATIVE*/,
    std::string_view dataPath/* = "Data/"*/, PathType dataPathType/* = PathType::RELATIVE_PATH*/)
    : Config()
{
    m_nWidth = width;
    m_nHeight = height;
    m_nWindowPosX = windowX;
    m_nWindowPosY = windowY;
    m_bFullScreen = fullscreen;
    setAssetPath(assetPath, assetPathType);
    setDataPath(dataPath, dataPathType);
}

bool Config::checkAndCorrectPath(std::string& strInOut, PathType pt) const noexcept
{
    if (strInOut.length() < 1)
        return false;

    switch (pt) {
        case PathType::ABSOLUTE_PATH:
            break;
        case PathType::RELATIVE_PATH:
        {
            std::filesystem::path p{ m_strBasePath };
            p.append(strInOut);
            strInOut = p.string();
        }
            break;
    }
    if (strInOut.back() != std::filesystem::path::preferred_separator)
        strInOut += std::filesystem::path::preferred_separator;
    
    
    if (!std::filesystem::exists(strInOut))
        return false;
 
    return true;
}

bool Config::setAssetPath(std::string_view newPath, PathType assetPathType) noexcept {
    
    std::string strNewPath = newPath.data();

    if (!checkAndCorrectPath(strNewPath, assetPathType))
        return false;
   
    m_strAssetPath = strNewPath;
    return true;
}

bool Config::setDataPath(std::string_view newPath, PathType dataPathType) noexcept {
    std::string strNewPath = newPath.data();

    if (!checkAndCorrectPath(strNewPath, dataPathType))
        return false;
   
    m_strDataPath = strNewPath;
    return true;
}

std::string Config::prependAssetPath(std::string_view fileName) const noexcept {
    std::filesystem::path p{ m_strAssetPath };
    p.append(fileName);
    return p.string();
}

std::string Config::prependBasePath(std::string_view fileName) const noexcept {
    std::filesystem::path p{ m_strBasePath };
    p.append(fileName);
    return p.string();
}

std::string Config::prependDataPath(std::string_view fileName) const noexcept {
    std::filesystem::path p{ m_strDataPath };
    p.append(fileName);
    return p.string();
}
}

/*
namespace Ice {

int Config::posX = -1;
int Config::posY = -1;
int Config::width = -1;
int Config::height = -1;
bool Config::fullscreen = false;
int Config::currentProgramId = 0;

}
*/
