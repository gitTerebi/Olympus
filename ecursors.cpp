#include "ecursors.h"

#include "egamedir.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Cursor* eCursors::sDefault = nullptr;
SDL_Cursor* eCursors::sShovel  = nullptr;
SDL_Cursor* eCursors::sRepairMallet = nullptr;

SDL_Cursor* eCursors::load(const char* filename, int hotX, int hotY) {
    const auto path = eGameDir::cursorsDir() + filename;
    SDL_Surface* surf = IMG_Load(path.c_str());
    if(!surf) {
        printf("Could not load cursor '%s': %s\n", path.c_str(), IMG_GetError());
        return nullptr;
    }
    SDL_Cursor* cursor = SDL_CreateColorCursor(surf, hotX, hotY);
    SDL_FreeSurface(surf);
    if(!cursor) printf("Could not create cursor '%s': %s\n", filename, SDL_GetError());
    return cursor;
}

bool eCursors::initialize() {
    sDefault = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    sShovel  = load("shovel_5_26.png", 5, 26);
    sRepairMallet = load("repair_5_26.png", 5, 26);
    return sShovel != nullptr && sRepairMallet != nullptr;
}

void eCursors::destroy() {
    SDL_FreeCursor(sDefault);
    SDL_FreeCursor(sShovel);
    SDL_FreeCursor(sRepairMallet);
    sShovel  = nullptr;
    sRepairMallet = nullptr;
    sDefault = nullptr;
}

void eCursors::set(const eCursorType type) {
    switch(type) {
    case eCursorType::defaultCursor:
        if(sDefault) SDL_SetCursor(sDefault);
        break;
    case eCursorType::shovel:
        if(sShovel) SDL_SetCursor(sShovel);
        break;
    case eCursorType::repairMallet:
        if(sRepairMallet) SDL_SetCursor(sRepairMallet);
        break;
    }
}
