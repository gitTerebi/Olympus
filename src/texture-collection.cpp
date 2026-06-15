#include "texture-collection.h"
#include "widgets/epainter.h"

TextureCollection::TextureCollection(SDL_Renderer* const r) :
    mRenderer(r) {

}

void TextureCollection::draw(ePainter& p,
                              const int x, const int y,
                              const int id) const {
    p.drawTexture(x, y, mTexs[id]);
}

void TextureCollection::draw(ePainter& p,
                              const int x, const int y,
                              const int id,
                              const Alignment align) const {
    p.drawTexture(x, y, mTexs[id], align);
}

std::shared_ptr<Texture>& TextureCollection::addTexture() {
    return mTexs.emplace_back(std::make_shared<Texture>());
}

std::shared_ptr<Texture> TextureCollection::loadTexture(
        const std::string& path) {
    const auto t = std::make_shared<Texture>();
    const bool r = t->load(mRenderer, path);
    if(!r) return nullptr;
    mTexs.push_back(t);
    return mTexs.back();
}

const std::shared_ptr<Texture>&
TextureCollection::getTexture(const int id) const {
    return mTexs[id];
}

int TextureCollection::size() const {
    return mTexs.size();
}
