//
//  packedVertex.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef packedVertex_h
#define packedVertex_h

#include <cstring>
#include <cmath>

namespace Ice {

template<int Dim = 3> struct vertexComponent {
    float data[Dim];

    static constexpr float EPSILON = .00001f;
    
    vertexComponent() noexcept {
	    std::memset(data, '\0', sizeof(float) * Dim);
    }

    bool operator==(const vertexComponent<Dim>& other) const {
        for (int i = 0; i < Dim; ++i) {
            if (data[i] < other.data[i] - EPSILON || data[i] > other.data[i] + EPSILON)
                return false;
        }
        return true;
    }

    bool operator!=(const vertexComponent<Dim>& other) const {
        return !(*this == other);
    }

    float& operator[](int nIndex) {
        return data[nIndex];
    }

	float length() const noexcept {
		float fSum{ 0.0f };
		for (int i = 0; i < Dim; ++i)
			fSum += data[i] * data[i];
		return std::sqrt(fSum);
	}

	//bool operator<(const vertexComponent<Dim>& other) const {
	//	bool bRet = true;
	//	for (int i = 0; i < Dim; ++i) {
	//		bRet &= data[i] < other.data[i];
	//		if (!bRet)
	//			return false;
 //       }
	//	return bRet;
	//}
};

struct packedVertex {
    vertexComponent<2> texCoord;
    vertexComponent<> vertex, normal;

    bool operator==(const packedVertex& other) const {
        return (vertex == other.vertex && normal == other.normal && texCoord == other.texCoord);
    }

    bool operator!=(const packedVertex& other) const {
        return !(*this == other);
    }

	//bool operator<(const packedVertex& other) const {
	//	return vertex < other.vertex && normal < other.normal && texCoord < other.texCoord;
	//}

	float length() const noexcept {
		return vertex.length() + normal.length() + texCoord.length();
	}
};

}

namespace std {

  template <>
  struct hash<Ice::vertexComponent<2>>
  {
    std::size_t operator()(const Ice::vertexComponent<2>& k) const
    {
      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

    size_t h1 = std::hash<double>()(k.data[0]);
    size_t h2 = std::hash<double>()(k.data[1]);
    return (h1 ^ (h2 << 1));
    }
  };

  template <>
  struct hash<Ice::vertexComponent<3>>
  {
    std::size_t operator()(const Ice::vertexComponent<3>& k) const
    {
      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

    size_t h1 = std::hash<double>()(k.data[0]);
    size_t h2 = std::hash<double>()(k.data[1]);
    size_t h3 = std::hash<double>()(k.data[2]);
    return (h1 ^ (h2 << 1)) ^ h3;
    }
  };

  template <>
  struct hash<Ice::packedVertex>
  {
    std::size_t operator()(const Ice::packedVertex& k) const
    {
      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

    size_t h1 = std::hash<Ice::vertexComponent<3>>()(k.vertex);
    size_t h2 = std::hash<Ice::vertexComponent<3>>()(k.normal);
    size_t h3 = std::hash<Ice::vertexComponent<2>>()(k.texCoord);
    return (h1 ^ (h2 << 1)) ^ h3;
    }
  };
}

#endif /* packedVertex_h */
