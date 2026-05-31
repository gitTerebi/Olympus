#ifndef FRAMED_BUTTON_WITH_ICON_H
#define FRAMED_BUTTON_WITH_ICON_H

#include "framed-button.h"

enum class eResourceType;

class FramedButtonWithIcon : public FramedButton {
public:
    using FramedButton::FramedButton;

    void initialize(const std::shared_ptr<eTexture>& icon,
                    const std::string& text);

    void initialize(const eResourceType type,
                    const std::string& text);
};

#endif // FRAMED_BUTTON_WITH_ICON_H
