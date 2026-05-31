#ifndef ERESOURCEBUTTON_H
#define ERESOURCEBUTTON_H

#include "framed-button.h"

#include "engine/eresourcetype.h"

class eResourceButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using eResourceAction = std::function<void(const eResourceType)>;
    void initialize(const eResourceAction& ract,
                    const eResourceType res =
                        eResourceType::allBasic,
                    const bool showFood = false,
                    const bool showNone = false);

    eResourceType resource() const { return mRes; }
    void setResource(const eResourceType res);
private:
    eResourceType mRes;
};

#endif // ERESOURCEBUTTON_H
