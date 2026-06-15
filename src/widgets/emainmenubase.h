#ifndef EMAINMENUBASE_H
#define EMAINMENUBASE_H

#include "elabel.h"

class eMainMenuBase : public eLabel {
public:
    using eLabel::eLabel;

    void renderTargetsReset() override;
    void initialize();
protected:
    void paintEvent(ePainter& p) override;
private:
    void updateBackgroundCache(ePainter& p);

    std::shared_ptr<Texture> mBackgroundCache;
};

#endif // EMAINMENUBASE_H
