#ifndef ECHECKABLEBUTTON_H
#define ECHECKABLEBUTTON_H

#include "ebutton.h"

using eCheckAction = std::function<void(bool)>;

class eCheckableButton : public eButton {
public:
    eCheckableButton(MainWindow* const window);

    bool checked() const { return mChecked; }

    void setCheckAction(const eCheckAction& a);

    void setChecked(const bool c);
    void check();
    void setCheckedTexture(const std::shared_ptr<Texture>& tex);
    void setCheckedHoverTexture(const std::shared_ptr<Texture>& tex);

    static eCheckableButton* sCreate(
            const TextureCollection& texs,
            MainWindow* const window,
            eWidget* const buttons = nullptr);
protected:
    void paintEvent(ePainter& p);
private:
    using eButton::setPressAction;
private:
    std::shared_ptr<Texture> mCheckedTexture;
    std::shared_ptr<Texture> mCheckedHoverTexture;
    bool mChecked = false;
    eCheckAction mCheckAction;
};

#endif // ECHECKABLEBUTTON_H
