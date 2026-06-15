#ifndef TEXTURE_COLLECTION_H
#define TEXTURE_COLLECTION_H

#include "texture.h"

class ePainter;

class TextureCollection {
public:
    TextureCollection(SDL_Renderer* const r);

    void draw(ePainter& p,
              const int x, const int y,
              const int id) const;
    void draw(ePainter& p,
              const int x, const int y,
              const int id,
              const Alignment align) const;
    std::shared_ptr<Texture>& addTexture();
    std::shared_ptr<Texture> loadTexture(const std::string& path);
    const std::shared_ptr<Texture>& getTexture(const int id) const;
    int size() const;
private:
    SDL_Renderer* const mRenderer;
    std::vector<std::shared_ptr<Texture>> mTexs;
};

#endif // TEXTURE_COLLECTION_H
