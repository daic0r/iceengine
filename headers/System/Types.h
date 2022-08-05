//
//  Types.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Types_h
#define Types_h

#include <array>
#include "Config.h"
#include <cstdint>

namespace Ice {

enum FaceDirection {
    eFront,
    eBack,
    eLeft,
    eRight,
    eBottom,
    eTop
};

enum BlockType {
    eAir,
    eGrass,
    eStone,
    eDirt,
    eCobble,
    eGravel,
    eSand,
    eLeaves,
    eLog,
    eSnow,
    eWater,
    eCactus,
    eFlowerRedDecoration,
    eFlowerYellowDecoration,
    eNone
};

enum Biome {
    eOcean,
    eDesert,
    eGrassland,
    eWinterland,
    eRockland
};

enum class OrthogonalDirection : std::uint8_t {
    NONE,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};

enum class EdgeOrientation : std::uint8_t {
    WITH_PATH,
    AGAINST_PATH,
    BOTH
};

enum class OrthogonalAxis {
    NONE,
    HORIZONTAL,
    VERTICAL
};

enum class MouseButton : std::uint8_t {
    NONE = 1,
    LEFT = 2,
    MIDDLE = 4,
    RIGHT = 8
};

using TFace = std::array<float, 12>;
using TBlockFaces = std::array<TFace, 6>;
static constexpr int HeightMapDim = 256;
using HeightMap = std::array<std::array<float, HeightMapDim>, HeightMapDim>;

}

#endif /* Types_h */
