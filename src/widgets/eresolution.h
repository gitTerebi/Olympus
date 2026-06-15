#ifndef ERESOLUTION_H
#define ERESOLUTION_H

#include <string>
#include <vector>

enum class eUIScale {
    tiny, small, medium, large
};

class eResolution {
public:
    eResolution() {}
    eResolution(const int width, const int height);
    eResolution(const int width, const int height, const eUIScale uiScale);

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    double multiplier() const;

    int paddingXL() const;
    int paddingL() const;
    int paddingM() const;
    int paddingS() const;
    int paddingXS() const;
    int paddingXXS() const;

    int margin() const;

    int fontSizeXL() const;
    int fontSizeL() const;
    int fontSizeM() const;
    int fontSizeS() const;
    int fontSizeXS() const;
    int fontSizeXXS() const;

    int centralWidgetLargeWidth() const;
    int centralWidgetSmallWidth() const;
    int centralWidgetLargeHeight() const;
    int centralWidgetSmallHeight() const;

    static std::vector<eResolution> sResolutions;

    const std::string& name() const { return mName; }
    eUIScale uiScale() const { return mUIScale; }

    bool operator==(const eResolution other) const {
        return mWidth == other.mWidth &&
               mHeight == other.mHeight;
    }

    bool operator!=(const eResolution other) const {
        return !(*this == other);
    }
private:
    int mWidth;
    int mHeight;
    eUIScale mUIScale;
    std::string mName;
};

#endif // ERESOLUTION_H
