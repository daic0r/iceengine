//
//  TerrainGenerator.h
//  IceCraft
//
//  Created by Matthias Gruen on 22.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainGenerator_h
#define TerrainGenerator_h

#include <System/Config.h>
#include <array>
#include <System/PerlinNoise.h>
#include <System/Tools.h>
#include <random>
#include <utility>

class Chunk;

namespace Ice {

#define terrainGenerator TerrainGenerator::instance()

class TerrainGenerator {
private:
    //std::array<std::array<float, HeightMapDim>, HeightMapDim> m_heightMap;
    float m_fWexelWidth{ -1.0f }, m_fWexelHeight{ -1.0f };
    PerlinNoise m_noiseGen;
    PerlinNoise m_biomeGen;
    std::mt19937 m_randomGenerator;
    int m_nLastX, m_nLastZ;
    
    TerrainGenerator();
    float treeNoise(float x, float y);
    float noise(float x, float y);
    float biomeNoise(float x, float y);
    
public:
    
    static TerrainGenerator& instance();
    
    //void generateTerrainAround(int x, int z);
    HeightMap generateNoiseMap(int x, int z, const std::function<float(float, float)>&);
    HeightMap generateHeightMap(int x, int z);
    HeightMap generateMoistureMap(int x, int z);
    static Biome getBiome(float fEleveation, float fMoisture);
    //void fillChunk(Chunk& c);
    //void placeTrees(int chunkX, int chunkZ);
    
    
};

}

#endif /* TerrainGenerator_h */
