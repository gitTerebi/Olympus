#ifndef EMULTILINELABEL_H
#define EMULTILINELABEL_H

#include "ewidget.h"

class eLabel;

class eMultiLineLabel : public eWidget {
public:
    using eWidget::eWidget;

    void setFontSizeXXS();
    void setFontSizeXS();
    void setFontSizeS();
    void setFontSizeM();
    void setFontSizeL();
    void setFontSizeXL();
    void setFontSize(const int s);

    void setLightFontColor();
    void setDarkFontColor();
    void setYellowFontColor();
    void setFontColor(const FontColor color);

    void setText(const std::string& text);

    void clear();
private:
    const eResolution& res() const;

    int mFontSize = 0;
    FontColor mFontColor = FontColor::light;
    std::vector<eLabel*> mLabels;
};

#endif // EMULTILINELABEL_H
