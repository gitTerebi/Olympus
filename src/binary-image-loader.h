#ifndef BINARY_IMAGE_LOADER_H
#define BINARY_IMAGE_LOADER_H

#include "texture.h"

class BinaryImageLoader {
public:
    static std::shared_ptr<Texture> load(SDL_Renderer* const r,
                                          const std::string& path);
};

#endif // BINARY_IMAGE_LOADER_H
