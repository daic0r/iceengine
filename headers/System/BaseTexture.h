#ifndef BaseTexture_h
#define BaseTexture_h

#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Rect;

namespace Ice {

enum class MinMagFilter {
	NONE,
	NEAREST,
	LINEAR
};

class BaseTexture
{
	int m_nWidth{ -1 }, m_nHeight{ -1 };
	float m_fTexelWidth{ -1.0f }, m_fTexelHeight{ -1.0f };

	struct sSubTextureAttributes {
		std::vector<float> vTexCoords;
		bool bHasTransparency;
	};

	std::string m_strName;
	std::string m_strFilename;
	std::unordered_map<std::string, sSubTextureAttributes> m_mSubTextures;
	bool m_bHasTransparency;

	int m_nRows{ -1 }, m_nCols{ -1 };
	mutable float m_fCellWidth{ -1.0f }, m_fCellHeight{ -1.0f };
	MinMagFilter m_minMagFilter{ MinMagFilter::LINEAR };

public:

	inline void setWidth(int width) noexcept { m_nWidth = width; }
	inline void setHeight(int height) noexcept { m_nHeight = height; }
	inline void setTexelWidth(float width) noexcept { m_fTexelWidth = width; }
	inline void setTexelHeight(float height) noexcept { m_fTexelHeight = height; }
	inline void setRows(int nRows) noexcept { m_nRows = nRows; };
	inline void setCols(int nCols) noexcept { m_nCols = nCols; }
	inline void setTransparency(bool b) noexcept { m_bHasTransparency = b; }

	inline float texelWidth() const noexcept { return m_fTexelWidth; }
	inline float texelHeight() const noexcept { return m_fTexelHeight; }
	inline int width() const noexcept { return m_nWidth; }
	inline int height() const noexcept { return m_nHeight; }
	inline int rows() const noexcept { return m_nRows; };
	inline int cols() const noexcept { return m_nCols; }
	inline bool hasTransparency() const noexcept { return m_bHasTransparency; }
	float cellWidth() const noexcept;
	float cellHeight() const noexcept;
	void setMinMagFilter(MinMagFilter fil) noexcept { m_minMagFilter = fil; }
	MinMagFilter minMagFilter() const noexcept { return m_minMagFilter; }

	std::pair<float, float> getTexCoord(int x, int y) const noexcept;
	bool hasTransparency(const std::string& name) const;

	void registerSubTexture(const std::string& name, const SDL_Rect&, bool bHasTransparency = false);
	const std::vector<float>& getTexCoord(const std::string& name) const;

	virtual void loadFromFile(const std::string&) = 0;
};

}

#endif