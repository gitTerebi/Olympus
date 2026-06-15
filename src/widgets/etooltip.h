#ifndef ETOOLTIP_H
#define ETOOLTIP_H

#include "efonts.h"
#include "texture.h"
#include "epainter.h"
#include "pointers/estdselfref.h"

class MainWindow;

class eTooltip {
public:
    eTooltip(MainWindow& w);

    void update();
    void paint(const int x, const int y, ePainter& p);

    int width() const;
    int height() const;

    bool empty() const;
private:
    int padding() const;

    MainWindow& mWindow;
    int mFontSize = -1;
    TTF_Font* mFont = nullptr;
    std::string mText;
    stdsptr<Texture> mTexture = std::make_shared<Texture>();
};

#endif // ETOOLTIP_H
