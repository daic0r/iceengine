#pragma once

#include <SDL2/SDL.h>
#include <string>

struct SDL_Surface;

namespace Ice {

struct SDLTools {
	
	static std::pair<SDL_Surface*, Uint8> readFileToSurface(const std::string& strFile, bool bFlip = true) noexcept;
};

}
