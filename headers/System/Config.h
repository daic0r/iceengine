//
//  Config.h
//  IceCraft
//
//  Created by Matthias Gruen on 07.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Config_h
#define Config_h

//#include <IceEngine.h>
#include <string_view>
#include <string>
#include <System/CameraConfig.h>
#include <Components/SystemIds.h>

namespace Ice {

enum class PathType {
    ABSOLUTE_PATH,
    RELATIVE_PATH
};

class Config {

    std::string m_strGameName;
    std::string m_strBasePath;
    std::string m_strAssetPath;
    std::string m_strDataPath;
    int m_nWidth{ 1024 }, m_nHeight{ 768 };
    int m_nWindowPosX{ -1 }, m_nWindowPosY{ -1 };
    bool m_bFullScreen{ false };
    float m_fWaterLevel;
    std::vector<SystemId> m_vSystemIds;

    CameraConfig m_camera;
    
public:

    Config();
    Config(int width, int height, bool fullscreen, int windowX = -1, int windowY = -1,
        std::string_view assetPath = "Assets/", PathType assetPathType = PathType::RELATIVE_PATH,
        std::string_view dataPath = "Data/", PathType dataPathType = PathType::RELATIVE_PATH
        );
    const std::string& basePath() const noexcept { return m_strBasePath; }
    
    std::string_view assetPath() const noexcept { return m_strAssetPath; }
    bool setAssetPath(std::string_view, PathType) noexcept;

    std::string_view dataPath() const noexcept { return m_strDataPath; }
    bool setDataPath(std::string_view, PathType) noexcept; 

    std::string_view gameName() const noexcept { return m_strGameName; }
    void setGameName(std::string_view strName) noexcept { m_strGameName = strName; }
    
    bool fullscreen() const noexcept { return m_bFullScreen; }
    int width() const noexcept { return m_nWidth; }
    int height() const noexcept { return m_nHeight; }
    int windowPosX() const noexcept { return m_nWindowPosX; }
    int windowPosY() const noexcept{ return m_nWindowPosY; }
    
    std::string prependAssetPath(std::string_view) const noexcept;
    std::string prependBasePath(std::string_view) const noexcept;
    std::string prependDataPath(std::string_view) const noexcept;

    auto& camera() noexcept { return m_camera; }

    const auto& systemIds() const noexcept { return m_vSystemIds; }
    void setSystemIds(const std::vector<SystemId>& v) { m_vSystemIds = v; }

private:
    bool checkAndCorrectPath(std::string& strInOut, PathType pt) const noexcept;
};



/*
namespace Config {

#ifdef WINDOWS_SYSTEM
    constexpr std::string_view BasePath = "../IceCraft/";
#else
    constexpr std::string_view BasePath = "/Users/matthias/Documents/programming/Games/IceCraft/IceCraft/";
#endif    
    constexpr int CHUNK_DIM = 16;
    constexpr int surfaceLevel = 5;
    constexpr int WORLD_HEIGHT = 256;    // 256 blocks
    constexpr int WORLD_DIM = 4096;
    constexpr int WATER_LEVEL = 35;

    extern int posX, posY;
    extern int width, height;
    extern bool fullscreen;
    extern int currentProgramId;

};
*/

}

#endif /* Config_h */
