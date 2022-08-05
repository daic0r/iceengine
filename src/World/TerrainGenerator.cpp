//
//  TerrainGenerator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 22.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include "TerrainGenerator.h"
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <random>
//#include "Controller.h"
#include "Tools.h"
#include "World.h"
#include "PerlinNoise.h"

namespace Ice {

TerrainGenerator::TerrainGenerator() {
    m_fWexelWidth =  1.0f / (Config::WORLD_DIM / 256);
    m_fWexelHeight = 1.0f / (Config::WORLD_DIM / 256);
    
    std::mt19937::result_type seed = time(0);
    m_randomGenerator = std::mt19937(seed);
//    auto rand = std::bind(std::uniform_int_distribution<int>(1,0xFF),
//                               std::mt19937(seed));
    
    
    
    m_noiseGen = PerlinNoise( 1337 );
    m_biomeGen = PerlinNoise( 1983 );
    
    m_nLastX = std::numeric_limits<int>::min();
    m_nLastZ = std::numeric_limits<int>::max();

    //
//    for (int i = 0; i < 1024; ++i) {
//        for (int j = 0; j < 1024; ++j) {
//            float x = (float) rand() / 256.0f;
//            float y = (float) rand() / 256.0f;
//            Gradient[i][j][0] = x;
//            Gradient[i][j][1] = y;
//        }
//    }
}

TerrainGenerator& TerrainGenerator::instance() {
    static TerrainGenerator _inst;
    return _inst;
}

HeightMap TerrainGenerator::generateNoiseMap(int chunkX, int chunkZ, const std::function<float(float, float)>& noiseFunc) {
    std::array<std::array<float, HeightMapDim>, HeightMapDim> heightMap;
    float fMin = 100, fMax = -100;
    
    float fStepX = m_fWexelWidth / (float) HeightMapDim;
    float fStepY = m_fWexelHeight / (float) HeightMapDim;
    for (int y = 0; y < HeightMapDim; ++y) {
        for (int x = 0; x < HeightMapDim; ++x) {
           
            float fArgX = (chunkX * m_fWexelWidth) + fStepX * x;
            float fArgY = (chunkZ * m_fWexelHeight) + fStepY * y;

            heightMap[y][x] = noiseFunc(fArgX, fArgY) * 256.0f; // ((glm::perlin(glm::vec2{ 50.0f * fArgX, 50.0f * fArgY }) + 1.0f) / 2.0f) * 256.0f;
            if (heightMap[y][x] < fMin) {
                fMin = heightMap[y][x];
            }
            if (heightMap[y][x] > fMax) {
                fMax = heightMap[y][x];
            }
        }
    }
    
//    std::cout << "Max perlin value: " << fMax << std::endl;
//    std::cout << "Min perlin value: " << fMin << std::endl;
    
    return heightMap;

}

HeightMap TerrainGenerator::generateHeightMap(int x, int z) {
    return generateNoiseMap(x, z, std::bind(&TerrainGenerator::noise, this, std::placeholders::_1, std::placeholders::_2));
}

HeightMap TerrainGenerator::generateMoistureMap(int x, int z) {
    return generateNoiseMap(x, z, std::bind(&TerrainGenerator::biomeNoise, this, std::placeholders::_1, std::placeholders::_2));
}

//void TerrainGenerator::generateTerrainAround(int x, int z) {
//    // Generate 1 row and column more than we render below
//    // That way we make sure that every chunk already has all of its surrounding chunks
//    // when the mesh is built
//    
//    if (x == m_nLastX && z == m_nLastZ)
//        return;
//    
//    std::vector<std::pair<int,int>> vNewChunks;
//    
////    auto createFunc = [&vNewChunks](int x, int z, int distX, int distZ) {
////        if (!world.hasChunkAt(x + distX, z + distZ)) {
////            world.chunkAt(x + distX, z + distZ);
////            vNewChunks.emplace_back(std::make_pair(x + distX, z + distZ));
////        }
////    };
//   
//    for (int loopZ = z-World::NumChunksZ; loopZ < z+World::NumChunksZ; ++loopZ) {
//        for (int loopX = x-World::NumChunksX; loopX < x+World::NumChunksX; ++loopX) {
//            if (Controller::instance().m_bDone)
//                return;
//            if (!world.hasChunkAt(loopX, loopZ)) {
//                world.chunkAt(loopX, loopZ);
//                vNewChunks.emplace_back(std::make_pair(loopX, loopZ));
//            }
//        }
//    }
//    
//    for (const auto& p : vNewChunks) {
//        placeTrees(p.first, p.second);
//    }
//    
////    for (int loopZ = z-World::NumChunksZ+1; loopZ < z+World::NumChunksZ-1; ++loopZ) {
////        for (int loopX = x-World::NumChunksX+1; loopX < x+World::NumChunksX-1; ++loopX) {
////            if (Controller::instance().m_bDone)
////                return;
////            placeTrees(loopX, loopZ);
////        }
////    }
//    
//    for (int loopZ = z-World::NumChunksZ+1; loopZ < z+World::NumChunksZ-1; ++loopZ) {
//        for (int loopX = x-World::NumChunksX+1; loopX < x+World::NumChunksX-1; ++loopX) {
//            if (Controller::instance().m_bDone)
//                return;
//            auto& chunk =world.chunkAt(loopX, loopZ);
//            auto glModel = dynamic_cast<ChunkModel*>(chunk.model());
//            if (glModel == nullptr) {
//                auto pModel = std::make_unique<ChunkModel>();
//                glModel = pModel.get();
//                chunk.setModel(std::move(pModel));
//                chunk.model()->buildFromChunk(chunk);
//                chunk.setNeedsRebuild(false);
//            }
//        }
//    }
//    
//    m_nLastX = x;
//    m_nLastZ = z;
//}

Biome TerrainGenerator::getBiome(float fElevation, float fMoisture) {
    if (fElevation < Config::WATER_LEVEL)
        return eOcean;
    if (fElevation < 40.0f)
        return eDesert;
    
    if (fElevation < 128.0f) {
        if (fMoisture < 35.0f) {
            return eGrassland;
        }
        else {
            return eDesert;
        }
    }
    if (fElevation < 256.0f) {
        if (fMoisture < 50.0f) {
            return eRockland;
        }
        else {
            return eWinterland;
        }
    }
    return eGrassland;
}

//void TerrainGenerator::fillChunk(Chunk &c) {
//    auto posX = c.position().x / Config::CHUNK_DIM;
//    auto posZ =-c.position().z / Config::CHUNK_DIM;
//    auto heightMap = generateNoiseMap(c.position().x / Config::CHUNK_DIM, -c.position().z / Config::CHUNK_DIM, std::bind(&TerrainGenerator::noise, this, std::placeholders::_1, std::placeholders::_2));
//    auto biomeMap = generateNoiseMap(c.position().x / Config::CHUNK_DIM, -c.position().z / Config::CHUNK_DIM, std::bind(&TerrainGenerator::biomeNoise, this, std::placeholders::_1, std::placeholders::_2));
//    for (int loopX = 0; loopX < Config::CHUNK_DIM; ++loopX) {
//        for (int loopY = 0; loopY < Config::CHUNK_DIM; ++loopY) {
//            int height =  heightMap[loopY][loopX];
//            Biome biome = getBiome(heightMap[loopY][loopX], biomeMap[loopY][loopX]);
//            for (int y = 0; y <= height; ++y) {
//                BlockType t = eDirt;
//                switch (biome) {
//                    case eOcean:
//                        t = eWater;
//                        break;
//                    case eGrassland:
//                        if (y == height)
//                            t = eGrass;
//                        else
//                            t = eDirt;
//                        break;
//                    case eWinterland:
//                        t = eSnow;
//                        break;
//                    case eDesert:
//                        t = eSand;
//                        break;
//                    case eRockland:
//                        t = eStone;
//                        break;
//                }
////                if (y < 50) {
////                    t = eWater;
////                } else
////                if (y < 130 && y == height) {
////                    t = eGrass;
////                } else
////                if (y < 130) {
////                    t = eDirt;
////                }
////                else
////                if (y < 150) {
////                    t = eStone;
////                } else
////                if (y < 256) {
////                    t = eSnow;
////                }
//                c.blockAt(loopX, y, loopY).setType(t);
//            }
//            if (posX == 2654 && posZ == 1319) {
//                int kkk = 5;;
//            }
//
////            auto treeMapEntry = treeMap[loopY][loopX];
////            std::cout << "Tree map entry is " << treeMapEntry << std::endl;
////            if (treeMapEntry > .9f) {
////                 for (int y = height + 1; y < height + 11; ++y) {
////                     c.blockAt(loopX, y, loopY).setType(eLog);
////                 }
////            }
////
//
////            if (loopX == 0 && loopY == 0) {
////                for (int y = height + 1; y < height + 11; ++y) {
////                    c.blockAt(loopX, y, loopY).setType(eLog);
////                }
////            }
//        }
//    }
//}

float TerrainGenerator::noise(float x, float y) {
    float ret = 0.0f;
    float fMax = 0.0f;
    
    auto addOctave = [x, y, &ret, &fMax, this](float m) {
        ret += m_noiseGen.octave(m, x, y, 0.0f);
        fMax += 1.0f / m;
    };
    //float ret = 1/16.0f * glm::perlin(glm::vec2{ 16.0f * x, 16.0f * y }) + 1/8.0f * glm::perlin(glm::vec2{ 8.0f * x, 8.0f * y }) + 1/4.0f * glm::perlin(glm::vec2{ 4.0f * x, 4.0f * y });
    //float ret = glm::perlin(glm::vec2{ 50.0f * x, 50.0f * y });
    //addOctave(150.0f);
    //addOctave(50.0f);
    //addOctave(35.0f);
    //addOctave(10.5f);
    //addOctave(5.0f);
    //addOctave(20.0f);
    addOctave(1.0f);
    addOctave(2.0f);
    addOctave(4.0f);
    addOctave(8.0f);
    addOctave(16.0f);
    addOctave(32.0f);

    fMax = pow(fMax, 1.8f);
    ret = pow(ret, 1.8f);
    
    ret /= fMax;

//    ret += 1.0f;
//    ret /= 2.0f;

//    ret = pow(ret, 3.0f);

    return ret;
}

float TerrainGenerator::treeNoise(float x, float y) {
    return 1 / 50.0f * (m_noiseGen.noise(50.0f * x, 50.0f * y, 0.0f) + 1.0f) / 2.0f;
}

float TerrainGenerator::biomeNoise(float x, float y) {
    return m_biomeGen.octave(8, x, y);
}

//void TerrainGenerator::placeTrees(int chunkX, int chunkZ) {
//   // auto camPos = Controller::instance().camera().position();
//
////    int /* x, z,*/ startChunkX, startChunkZ;
////    //float blockX, blockY, blockZ;
////    //Tools::getChunkCoords(camPos, x, z, blockX, blockY, blockZ);
////
////    startChunkZ = chunkZ-World::NumChunksZ;
////    startChunkX = chunkX-World::NumChunksX;
////    auto &startChunk = world.chunkAt(startChunkX, startChunkZ);
////
//////    if (startChunk.model() != nullptr)
//////        return;
////
////    constexpr int numBlockZ = (2*World::NumChunksZ+1) * Config::CHUNK_DIM;
////    constexpr int numBlockX = (2*World::NumChunksX+1) * Config::CHUNK_DIM;
////
////    std::array<std::array<float, numBlockZ>, numBlockX> bluenoise;
////
////    float fStepX = m_fWexelWidth / (float) Config::CHUNK_DIM;
////    float fStepZ = m_fWexelHeight / (float) Config::CHUNK_DIM;
////
////    for (int loopZ = 0; loopZ < numBlockZ; ++loopZ) {
////        for (int loopX = 0; loopX < numBlockX; ++loopX) {
////            float nx = ((chunkX - World::NumChunksX) * m_fWexelWidth) + (float)loopX * fStepX, nz = ((chunkZ - World::NumChunksZ) * m_fWexelHeight) + (float)loopZ * fStepZ;
////            // blue noise is high frequency; try varying this
////            bluenoise[loopZ][loopX] = treeNoise(nx, nz);
////            //if (bluenoise[loopZ][loopX] != 0.0)
////            //    std::cout << "bluenoise is " << bluenoise[loopZ][loopX] << std::endl;
////        }
////    }
//   
//    auto& watchedChunk = world.chunkAt(chunkX, chunkZ);
//    
//    if (watchedChunk.m_bTreesPlaced)
//        return;
//
//    auto uni = std::uniform_int_distribution<>(0,1024);
//    int nTreeNum = uni(m_randomGenerator);
//    int nDecoNum = uni(m_randomGenerator);
//
//    
//    for (int loopZ = 0; loopZ < Config::CHUNK_DIM; ++loopZ) {
//        for (int loopX = 0; loopX < Config::CHUNK_DIM; ++loopX) {
//            if (Controller::instance().m_bDone)
//                return;
//            
//            auto res = uni(m_randomGenerator);
//            if (res != nTreeNum && res != nDecoNum)
//                continue;
//            
//            int y = watchedChunk.lastSolidLevel(loopX, loopZ);
//            const Block& topBlock = watchedChunk.blockAt(loopX, y, loopZ);
//
//            if (res == nDecoNum) {
//                if (nDecoNum < 512)
//                    watchedChunk.blockAt(loopX, y + 1, loopZ).setType(eFlowerRedDecoration);
//                else
//                    watchedChunk.blockAt(loopX, y + 1, loopZ).setType(eFlowerYellowDecoration);
//            }
//            else {
//                BlockType trunkBlockType;
//                switch (topBlock.type()) {
//                    case eSand:
//                        trunkBlockType = eCactus;
//                        break;
//                    case eGrass:
//                        trunkBlockType = eLog;
//                        break;
//                    default:
//                        continue;
//                }
//
//                for (int loopY = y + 1; loopY < y + 10; ++loopY) {
//                    watchedChunk.blockAt(loopX, loopY, loopZ).setType(trunkBlockType);
//                }
//                
//                if (trunkBlockType == eLog) {
//                    for (int tempZ = loopZ - 3; tempZ <= loopZ + 3; ++tempZ) {
//                        for (int tempX = loopX - 3; tempX <= loopX + 3; ++tempX) {
//                            for (int loopY = y + 8; loopY < y + 12; ++loopY) {
//                                if (Controller::instance().m_bDone)
//                                    return;
//
//                                watchedChunk.blockAt(tempX, loopY, tempZ).setType(eLeaves);
//
//                                int myTempX = tempX, myTempZ = tempZ;
//                                int thisChunkX, thisChunkZ;
//
//                                watchedChunk.getAdjacentChunkPos(myTempX, myTempZ, thisChunkX, thisChunkZ, myTempX, myTempZ);
//                                if (world.hasChunkAt(thisChunkX, thisChunkZ)) {
//                                    auto &realChunk = world.chunkAt(thisChunkX, thisChunkZ);
//                                    realChunk.determineLastSolidLevel(myTempX, myTempZ);
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            //watchedChunk.determineLastSolidLevels(true);
//            watchedChunk.determineLastSolidLevel(loopX, loopZ);
//            watchedChunk.m_bTreesPlaced = true;
//            return;
//        }
//    }
//}

}
