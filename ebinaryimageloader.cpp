#include "ebinaryimageloader.h"

#include <fstream>

#include "esplitbinary.h"
#include "egamedir.h"

std::shared_ptr<eTexture> eBinaryImageLoader::load(SDL_Renderer* const r,
                                                   const std::string& path) {
    {
        const auto slash = path.find('/');
        const std::string rel = (slash != std::string::npos) ? path.substr(slash + 1) : path;
        const std::string loosePath = eGameDir::exeDir() + "../textures/" + rel;
        std::ifstream loose(loosePath, std::ios::in | std::ios::binary);
        if(loose) {
            loose.close();
            printf("Loading loose texture: '%s'\n", loosePath.c_str());
            const auto tex = std::make_shared<eTexture>();
            tex->load(r, loosePath);
            return tex;
        }
    }

    const auto it = eBinaryDataMap.find(path);
    if(it == eBinaryDataMap.end()) {
        printf("Could not find '%s' image\n", path.c_str());
        return nullptr;
    }
    const auto& bd = it->second;
    std::string epath;
    switch(bd.fFileId) {
    case eFileId::i:
        epath = eGameDir::iBinaryPath();
        break;
    case eFileId::i15:
        epath = eGameDir::i15BinaryPath();
        break;
    case eFileId::i30:
        epath = eGameDir::i30BinaryPath();
        break;
    case eFileId::i45:
        epath = eGameDir::i45BinaryPath();
        break;
    case eFileId::i60:
        epath = eGameDir::i60BinaryPath();
        break;
    }

    std::ifstream file(epath, std::ios::in | std::ios::binary);
    if(!file) {
        printf("Could not open '%s'\n", epath.c_str());
        return nullptr;
    }

    const auto data = new char[bd.fSize];
    file.seekg(bd.fPos);
    file.read(data, bd.fSize);
    file.close();
    const auto rw = SDL_RWFromMem(data, bd.fSize);
    const auto surf = IMG_Load_RW(rw, SDL_FALSE);
    if(!surf) {
        printf("Unable to load image %s! SDL_image Error: %s\n",
               path.c_str(), IMG_GetError());
        return nullptr;
    }
    if(surf->format->BytesPerPixel == 4) {
        SDL_LockSurface(surf);
        Uint32* pixels = (Uint32*)surf->pixels;
        const Uint32 green = SDL_MapRGBA(surf->format, 0, 251, 0, 255);
        const Uint32 transparent = SDL_MapRGBA(surf->format, 0, 251, 0, 0);
        for(int i = 0; i < surf->w * surf->h; i++) {
            if(pixels[i] == green) pixels[i] = transparent;
        }
        SDL_UnlockSurface(surf);
    }
    delete[] data;

    const auto tex = std::make_shared<eTexture>();
    tex->load(r, surf);

    return tex;
}
