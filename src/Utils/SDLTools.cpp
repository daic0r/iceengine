#include <Utils/SDLTools.h>
#include <SDL2/SDL_image.h>

namespace Ice {

std::pair<SDL_Surface*, Uint8> SDLTools::readFileToSurface(const std::string& strFile, bool bFlip) noexcept {
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    auto tex_surface = IMG_Load(strFile.c_str());
    if (tex_surface == nullptr) {
        return std::make_pair((SDL_Surface*)nullptr, -1);
    }

    const auto bitsPerPixel = tex_surface->format->BitsPerPixel;
    auto pRetSurface = tex_surface;
   	if (bFlip) {
		auto converted_surface = SDL_CreateRGBSurface(0, tex_surface->w, tex_surface->h, tex_surface->format->BitsPerPixel, rmask, gmask, bmask, amask);
        if (converted_surface == nullptr) {
            return std::make_pair((SDL_Surface*)nullptr, -1);
        }
        for (int y = 0; y < tex_surface->h - 1; ++y) {
			SDL_Rect src_rect, dst_rect;
			src_rect.x = 0;
			src_rect.y = y;
			src_rect.w = tex_surface->w;
			src_rect.h = 1;
			dst_rect.x = 0;
			dst_rect.y = tex_surface->h - 1 - y;
			dst_rect.w = 0;
			dst_rect.h = 1;
			SDL_BlitSurface(tex_surface, &src_rect, converted_surface, &dst_rect);
		}
        pRetSurface = converted_surface;
        SDL_FreeSurface(tex_surface);
	}

    return std::make_pair(pRetSurface, bitsPerPixel);
}
}