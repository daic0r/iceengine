//
//  World.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <World/World.h>
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <glm/vec4.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <vector>
#include <Entities/Entity.h>

namespace Ice {

World::World(std::uint16_t nDim) {
    //initializeFromMap(strMapFile);
    m_nWorldSize = nDim;
    m_vTiles.resize(nDim);
    for (int z = 0; z < nDim; ++z) {
        m_vTiles[z].resize(nDim);
        //std::fill(m_vTiles[z].begin(), m_vTiles[z].end(), 0);
    }
}

std::map<Ice::OrthogonalDirection, std::vector<Ice::Entity>> World::getAdjacentEntities(const std::pair<int,int>& fromTile, const std::pair<int,int>& toTile) const noexcept {
    using Ice::Entity;
    
    std::map<Ice::OrthogonalDirection, std::vector<Ice::Entity>> mRet{};
    
    auto nLowerBoundX = fromTile.first;
    auto nLowerBoundZ = fromTile.second;
    auto nUpperBoundX = toTile.first;
    auto nUpperBoundZ = toTile.second;
    for (int x = nLowerBoundX; x <= nUpperBoundX; ++x) {
		if (nLowerBoundZ > 0) {
			if (Entity e = tile(x, nLowerBoundZ - 1); e > 0) {
				//std::cout << "Detected adjacent entity " << e << std::endl;
				mRet[Ice::OrthogonalDirection::TOP].emplace_back(e);
			}
		}
        if (Entity e = tile(x, nUpperBoundZ+1); e > 0) {
            //std::cout << "Detected adjacent entity " << e << std::endl;
            mRet[Ice::OrthogonalDirection::BOTTOM].emplace_back(e);
        }
    }
    for (int z = nLowerBoundZ; z <= nUpperBoundZ; ++z) {
		if (nLowerBoundX > 0) {
			if (Entity e = tile(nLowerBoundX - 1, z); e > 0) {
				//std::cout << "Detected adjacent entity " << e << std::endl;
				mRet[Ice::OrthogonalDirection::LEFT].emplace_back(e);
			}
		}
        if (Entity e = tile(nUpperBoundX+1, z); e > 0) {
            //std::cout << "Detected adjacent entity " << e << std::endl;
            mRet[Ice::OrthogonalDirection::RIGHT].emplace_back(e);
        }
    }
    
    return mRet;
}

}

//void World::initializeFromMap(const std::string& strMapFile) {
//    auto pSurface = IMG_Load(strMapFile.c_str());
//    if (pSurface == nullptr)
//        return;
//
//    Uint32 rmask, gmask, bmask, amask;
//#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//    rmask = 0xff000000;
//    gmask = 0x00ff0000;
//    bmask = 0x0000ff00;
//    amask = 0x000000ff;
//#else
//    rmask = 0x000000ff;
//    gmask = 0x0000ff00;
//    bmask = 0x00ff0000;
//    amask = 0xff000000;
//#endif
//
//    SDL_Surface *pConverted = SDL_CreateRGBSurface(0, pSurface->w, pSurface->h, pSurface->format->BitsPerPixel, rmask, gmask, bmask, amask);
//    SDL_BlitSurface(pSurface, nullptr, pConverted, nullptr);
//
//    glm::ivec4 colorTownHall{ 49, 162, 242, 255 };
//    glm::ivec4 colorHouse{ 190, 38, 51, 255 };
//    glm::ivec4 colorRoad{ 0, 0, 0, 255 };
//
//    std::map<TileType, std::vector<SDL_Rect>> mBuildingRects;
//
//    const int nStride = pConverted->format->BitsPerPixel / 8;
//    auto pixelAt = [pConverted, nStride](int x, int y) {
//        glm::ivec4 pixel;
//        for (int i = 0; i < nStride; ++i) {
//            pixel[i] = ((unsigned char*) pConverted->pixels)[(y * pConverted->w * nStride) + nStride * x + i];
//        }
//        return pixel;
//    };
//    for (int y = 0; y < pConverted->h; ++y) {
//        for (int x = 0; x < pConverted->w; ++x) {
//            auto checkInRect = [x, y, &mBuildingRects]() {
//                for (const auto& [t, vRects] : mBuildingRects) {
//                    for (const auto& rect : vRects) {
//                        SDL_Point p{ x, y };
//                        if (SDL_PointInRect(&p, &rect)) {
//                            return true;
//                        }
//                    }
//                }
//                return false;
//            };
//            if (checkInRect())
//                continue;
//
//            glm::ivec4 pixel = pixelAt(x, y);
//            TileType t;
//            if (pixel == colorTownHall) {
//                t = TileType::TOWNHALL;
//            } else
//            if (pixel == colorHouse) {
//                t = TileType::RESIDENTIAL_HOUSE;
//            } else
//            if (pixel == colorRoad) {
//                t = TileType::ROAD;
//            } else {
//                t = TileType::NONE;
//            }
//            if (t == TileType::NONE)
//                continue;
//
//            SDL_Rect r;
//            r.x = x; r.y = y;
//            int tx = x, ty = y;
//            while ((pixelAt(tx, ty) == pixel) && tx < pConverted->w)
//                ++tx;
//            r.w = tx - x;
//            tx = x;
//            while ((pixelAt(tx, ty) == pixel) && ty < pConverted->h)
//                ++ty;
//            r.h = ty - y;
//            mBuildingRects[t].emplace_back(r);
//        }
//    }
//
//    SDL_FreeSurface(pConverted);
//
//    for (const auto& [t, vRects] : mBuildingRects) {
//        for (const auto& rect : vRects) {
//            std::cout << "Type " << t << " at [" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << "]" << std::endl;
//        }
//    }
//}
