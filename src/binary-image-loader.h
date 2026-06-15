#ifndef BINARY_IMAGE_LOADER_H
#define BINARY_IMAGE_LOADER_H

#include "etexture.h"

class BinaryImageLoader {
public:
    static std::shared_ptr<eTexture> load(SDL_Renderer* const r,
                                          const std::string& path);
};

#endif // BINARY_IMAGE_LOADER_H
