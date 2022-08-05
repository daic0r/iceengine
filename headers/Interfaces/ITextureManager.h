#pragma once

#include <string>

namespace Ice {

class BaseTexture;

class ITextureManager {
public:
	virtual BaseTexture* registerTexture(const std::string& strFile) = 0;
	virtual BaseTexture* getTexture(const std::string& strFile) noexcept = 0;
};

}