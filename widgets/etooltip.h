#ifndef ETOOLTIP_H
#define ETOOLTIP_H

#include "efonts.h"
#include "etexture.h"
#include "epainter.h"
#include "pointers/estdselfref.h"

class eMainWindow;

class eTooltip {
public:
    eTooltip(eMainWindow& w);

    void update();
    void paint(const int x, const int y, ePainter& p);

    int width() const;
    int height() const;

    bool empty() const;
private:
    int padding() const;

    eMainWindow& mWindow;
    int mFontSize = -1;
    TTF_Font* mFont = nullptr;
    std::string mText;
    stdsptr<eTexture> mTexture = std::make_shared<eTexture>();
};

#endif // ETOOLTIP_H