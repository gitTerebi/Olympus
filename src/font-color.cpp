#include "font-color.h"

void FontColorHelpers::colors(const FontColor c,
                               SDL_Color& col1,
                               SDL_Color& col2) {
    switch(c) {
    case FontColor::light:
        col1 = SDL_Color{255, 255, 255, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    case FontColor::dark:
        col1 = SDL_Color{0, 0, 33, 255};
        col2 = SDL_Color{140, 210, 247, 255};
        break;
    case FontColor::yellow:
        col1 = SDL_Color{255, 207, 0, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    case FontColor::region:
        col1 = SDL_Color{168, 96, 80, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    case FontColor::red:
        col1 = SDL_Color{255, 0, 0, 255};
        col2 = SDL_Color{0, 0, 0, 255};
        break;
    }
}
