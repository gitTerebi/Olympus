#include "binary-image-loader.h"

#include "textures/sg-reader.h"

std::shared_ptr<eTexture> BinaryImageLoader::load(SDL_Renderer* const r,
                                                   const std::string& path) {
    // Sprites are decoded live from the player's own DATA/*.sg3 + *.555 - the old
    // loose textures/<zoom>/ PNG override is no longer used. SgReader prints the
    // specific failure reason on miss.
    const auto surf = SgReader::load(path);
    if(!surf) return nullptr;

    const auto tex = std::make_shared<eTexture>();
    tex->load(r, surf); // takes ownership of surf (frees it)

    return tex;
}
