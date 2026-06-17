#include "eresolution.h"

std::vector<eResolution> eResolution::sResolutions{
    eResolution{800, 600},
    eResolution{1024, 768},
    eResolution{1280, 720},
    eResolution{1360, 768},
    eResolution{1366, 768},
    eResolution{1600, 900},
    eResolution{1920, 1080},
    eResolution{2560, 1440},
    eResolution{3840, 2160},
    eResolution{1280, 800},
    eResolution{1440, 900},
    eResolution{1680, 1050},
    eResolution{1920, 1200},
    eResolution{2560, 1600},
    eResolution{1138, 600},
    eResolution{1259, 664},
    eResolution{1395, 736},
    eResolution{1532, 808},
    eResolution{1668, 880},
    eResolution{1805, 952},
    eResolution{1942, 1024},
    eResolution{2078, 1096},
    eResolution{2214, 1168},
    eResolution{2493, 1315},
    eResolution{1280, 1024},
    eResolution{2560, 1080},
    eResolution{3440, 1440},
};

eResolution::eResolution(const int width, const int height) :
    mWidth(width), mHeight(height) {
    if(height <= 800) {
        mUIScale = eUIScale::small;
    } else if(height <= 1200) {
        mUIScale = eUIScale::medium;
    } else {
        mUIScale = eUIScale::large;
    }
    mName = std::to_string(mWidth) + "x" + std::to_string(mHeight);
}

eResolution::eResolution(const int width, const int height,
                         const eUIScale uiScale) :
    mWidth(width), mHeight(height), mUIScale(uiScale) {
    mName = std::to_string(mWidth) + "x" + std::to_string(mHeight);
}

int eResolution::paddingXL() const {
    return 3*paddingL()/2;
}

int eResolution::paddingL() const {
    return 10*multiplier();
}

int eResolution::paddingM() const {
    return 3*paddingL()/4;
}

int eResolution::paddingS() const {
    return paddingL()/2;
}

int eResolution::paddingXS() const {
    return paddingL()/4;
}

int eResolution::paddingXXS() const {
    return paddingL()/6;
}

int eResolution::margin() const {
    return paddingL();
}

double eResolution::multiplier() const {
    switch(mUIScale) {
    case eUIScale::tiny:
        return 0.75;
    case eUIScale::small:
        return 1.0;
    case eUIScale::mediumSmall:
        return 1.25;
    case eUIScale::medium:
        return 1.5;
    case eUIScale::mediumLarge:
        return 1.75;
    case eUIScale::large:
        return 2.0;
    }
    return 1.0;
}

int eResolution::fontSizeXL() const {
    return 3*fontSizeL()/2;
}

int eResolution::fontSizeL() const {
    return 20*multiplier();
}

int eResolution::fontSizeM() const {
    return 3*fontSizeL()/4;
}

int eResolution::fontSizeS() const {
    return 3*fontSizeL()/5;
}

int eResolution::fontSizeXS() const {
    return 17*fontSizeL()/30;
}

int eResolution::fontSizeXXS() const {
    return 10*multiplier();
}

int eResolution::centralWidgetLargeWidth() const {
    return 660*multiplier();
}

int eResolution::centralWidgetSmallWidth() const {
    return 480*multiplier();
}

int eResolution::centralWidgetLargeHeight() const {
    return 540*multiplier();
}

int eResolution::centralWidgetSmallHeight() const {
    return 400*multiplier();
}
