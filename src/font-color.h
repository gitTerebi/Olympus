#ifndef FONT_COLOR_H
#define FONT_COLOR_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

enum class FontColor {
    light, dark, yellow, region, red
};

namespace FontColorHelpers {
    void colors(const FontColor c,
                SDL_Color& col1,
                SDL_Color& col2);
};

#endif // FONT_COLOR_H
