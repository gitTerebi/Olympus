#include "emultilinelabel.h"

#include "main-window.h"
#include "widgets/elabel.h"

#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split_string_by_newline(const std::string& str) {
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};

    for(std::string line; std::getline(ss, line, '*');) {
        result.push_back(line);
    }

    return result;
}

const eResolution& eMultiLineLabel::res() const {
    return window()->resolution();
}

void eMultiLineLabel::setFontSizeXXS() {
    const int s = res().fontSizeXXS();
    return setFontSize(s);
}

void eMultiLineLabel::setFontSizeXS() {
    const int s = res().fontSizeXS();
    return setFontSize(s);
}

void eMultiLineLabel::setFontSizeS() {
    const int s = res().fontSizeS();
    return setFontSize(s);
}

void eMultiLineLabel::setFontSizeM() {
    const int s = res().fontSizeM();
    return setFontSize(s);
}

void eMultiLineLabel::setFontSizeL() {
    const int s = res().fontSizeL();
    return setFontSize(s);
}

void eMultiLineLabel::setFontSizeXL() {
    const int s = res().fontSizeXL();
    return setFontSize(s);
}

void eMultiLineLabel::setFontSize(const int s) {
    mFontSize = s;
}

void eMultiLineLabel::setFontColor(const FontColor color) {
    mFontColor = color;
}

void eMultiLineLabel::setText(const std::string& text) {
    clear();
    const auto lines = split_string_by_newline(text);
    if(lines.empty()) return;
    for(const auto& l : lines) {
        const auto label = new eLabel(window());
        label->setFontSize(mFontSize);
        label->setFontColor(mFontColor);
        label->setNoPadding();
        label->setText(l);
        label->fitContent();
        addWidget(label);
        mLabels.push_back(label);
    }
    stackVertically();
    fitContent();
    for(const auto l : mLabels) {
        l->align(Alignment::hcenter);
    }
}

void eMultiLineLabel::clear() {
    for(const auto l : mLabels) {
        l->deleteLater();
    }
    mLabels.clear();
}

void eMultiLineLabel::setLightFontColor() {
    setFontColor(FontColor::light);
}

void eMultiLineLabel::setDarkFontColor() {
    setFontColor(FontColor::dark);
}

void eMultiLineLabel::setYellowFontColor() {
    setFontColor(FontColor::yellow);
}
