#ifndef EFRAMEDWIDGET_H
#define EFRAMEDWIDGET_H

#include "ewidget.h"

enum class eFrameType {
    outer, message, inner
};

class eFramedWidget : public eWidget {
public:
    using eWidget::eWidget;

    void renderTargetsReset() override;
    void setType(const eFrameType type);
protected:
    void paintEvent(ePainter& p) override;
private:
    void resetCache();

    eFrameType mType = eFrameType::outer;
    std::shared_ptr<Texture> mBackgroundCache;
};

#endif // EFRAMEDWIDGET_H
