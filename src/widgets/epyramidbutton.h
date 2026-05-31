#ifndef EPYRAMIDBUTTON_H
#define EPYRAMIDBUTTON_H

#include "framed-button.h"

#include "buildings/ebuilding.h"

class ePyramidButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using ePyramidAction = std::function<void(const eBuildingType)>;
    void initialize(const ePyramidAction& pact);

    eBuildingType type() const { return mType; }
    void setType(const eBuildingType type);
private:
    eBuildingType mType;
};

#endif // EPYRAMIDBUTTON_H
