#ifndef SG_READER_H
#define SG_READER_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

struct eSpriteData;

// Live decoder for Impressions SG3/555 sprite files (Zeus + Poseidon DATA/).
// Ships zero pre-bundled graphics: sprites are decoded at runtime from the
// player's own DATA/<group>.sg3 + DATA/<group>.555 pair.
//
// Two kinds of source:
//  - Passthrough: "30/SprAmbient/SprAmbient_00009.png" -> DATA/SprAmbient.sg3
//    record 9, scaled to the requested zoom.
//  - Composite: "45/interfaceBanners_0.png" -> the eZeus packed strip that
//    src/textureTemplates/interfaceBanners says is records [start..end] of a
//    named sub-group inside a .sg3. Records are decoded and blitted into a strip
//    surface at the rects the compiled spriteData (.h) holds, so existing
//    sub-rect lookups keep working.
class SgReader {
public:
    // Passthrough decode by eZeus texture path. Returns a freshly allocated
    // RGBA32 surface (caller owns), or nullptr when no SG source exists.
    static SDL_Surface* load(const std::string& path);

    // Composite strip builder. Decodes the textureTemplates/<name> record range
    // and blits each record into a strip surface at the spriteData rect for the
    // entries whose fTexId == texId. `sds` is the zoom-specific spriteData (its
    // array order matches the template's start..end range). Returns the strip
    // surface (caller owns) or nullptr if no template/SG source.
    static SDL_Surface* loadComposite(const std::string& name,
                                      const int tileH,
                                      const int texId,
                                      const std::vector<eSpriteData>& sds);
};

#endif // SG_READER_H
