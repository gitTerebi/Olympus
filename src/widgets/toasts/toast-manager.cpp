#include "widgets/game-widget.h"

#include "widgets/eflatbutton.h"
#include "string-helpers.h"
#include "characters/gods/god.h"
#include "characters/monsters/emonster.h"
#include "engine/eresourcetype.h"
#include "engine/game-board.h"
#include "widgets/message-action-refresh.h"

#include <algorithm>
#include <chrono>
#include <cstdio>

namespace {
class ToastButton : public eFlatButton {
public:
    using eFlatButton::eFlatButton;

    void renderTargetsReset() override {
        eLabel::renderTargetsReset();
        mNormalCache.reset();
        mHoverCache.reset();
    }
protected:
    void paintEvent(ePainter& p) override {
        auto& cache = hovered() ? mHoverCache : mNormalCache;
        const int bleedX = cacheBleedX();
        const int bleedTop = cacheBleedTop();
        const int bleedBottom = cacheBleedBottom();
        if(!cache ||
           cache->width() != width() + 2*bleedX ||
           cache->height() != height() + bleedTop + bleedBottom) {
            updateCache(p, cache);
        }
        if(cache) {
            cache->render(p.renderer(), p.x() - bleedX, p.y() - bleedTop);
            return;
        }
        eFlatButton::paintEvent(p);
    }
private:
    int cacheBleedX() const {
        return std::max(2, fontSize()/6);
    }
    int cacheBleedTop() const { return cacheBleedX(); }
    int cacheBleedBottom() const {
        return std::max(8, fontSize()/2);
    }

    void updateCache(ePainter& p, std::shared_ptr<Texture>& cache) {
        if(width() <= 0 || height() <= 0) return;
        const int bleedX = cacheBleedX();
        const int bleedTop = cacheBleedTop();
        const int bleedBottom = cacheBleedBottom();
        const int cacheW = width() + 2*bleedX;
        const int cacheH = height() + bleedTop + bleedBottom;
        const auto r = p.renderer();
        auto next = std::make_shared<Texture>();
        if(!next->create(r, cacheW, cacheH)) return;

        const auto prevTarget = SDL_GetRenderTarget(r);
        SDL_BlendMode prevBlendMode;
        SDL_GetRenderDrawBlendMode(r, &prevBlendMode);

        next->setAsRenderTarget(r);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
        SDL_RenderClear(r);
        SDL_SetRenderDrawBlendMode(r, prevBlendMode);

        ePainter cachePainter(r);
        cachePainter.translate(bleedX, bleedTop);
        eFlatButton::paintEvent(cachePainter);

        SDL_SetRenderTarget(r, prevTarget);
        SDL_SetRenderDrawBlendMode(r, prevBlendMode);
        cache = next;
    }

    std::shared_ptr<Texture> mNormalCache;
    std::shared_ptr<Texture> mHoverCache;
};
}

void GameWidget::createToastWidget(eToast &toast)
{
    const auto start = std::chrono::steady_clock::now();
    eEventData edCopy = toast.fEd;
    const auto msg = toast.fMsg;
    eFlatButton* tw = new ToastButton(window());
    tw->setNoPadding();
    tw->setFontSizeXS();
    auto title = msg.fTitle;
    if (const auto &c = toast.fEd.fCity)
        StringHelpers::replaceAll(title, "[city_name]", c->name());
    if (const auto &c = toast.fEd.fRivalCity)
        StringHelpers::replaceAll(title, "[rival_city_name]", c->name());
    StringHelpers::replaceAll(title, "[item]",
                               eResourceTypeHelpers::typeLongName(toast.fEd.fResourceType));
    StringHelpers::replaceAll(title, "[itemshort]",
                               eResourceTypeHelpers::typeName(toast.fEd.fResourceType));
    StringHelpers::replaceAll(title, "[god]", God::sGodName(toast.fEd.fGod));
    StringHelpers::replaceAll(title, "[monster]", eMonster::sMonsterName(toast.fEd.fMonster));
    if (title.length() > 40)
        title = title.substr(0, 37) + "...";
    tw->setText(title);
    tw->fitContent();
    tw->setPressAction([this, edCopy, msg, tw]() mutable
    {
        for (int i = 0; i < int(mToasts.size()); i++)
        {
            if (mToasts[i].fWid == tw)
            {
                mToasts.erase(mToasts.begin() + i);
                break;
            }
        }
        tw->deleteLater();
        updateToastPositions();
        refreshMessageActions(mBoard, edCopy);
        showMessage(edCopy, msg, false, true, false);
    });
    const int p = tw->padding();
    tw->resize(tw->width() + 2 * p, tw->height() + 2 * p);
    const int vw = width() - mGm->width();
    tw->setX((vw - tw->width()) / 2);
    toast.fWid = tw;
    addWidget(tw);
    const auto end = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration<double, std::milli>(end - start).count();
    if (ms > 0.5)
        printf("toast create %.2f ms title='%s'\n", ms, title.c_str());
}

void GameWidget::showToast(eEventData &ed, const Message &msg)
{
    eToast pendingToast;
    pendingToast.fEd = ed;
    pendingToast.fMsg = msg;
    pendingToast.fDate = mBoard->date();
    pendingToast.fExpireTick = 0;
    pendingToast.fQueued = false;
    mPendingToasts.push_back(pendingToast);
}

void GameWidget::updateToastPositions()
{
    const int p = padding();
    int y;
    if (mPausedLabel)
    {
        y = mPausedLabel->y() + mPausedLabel->height() + 2 * p;
    }
    else
    {
        y = 5 * p;
    }
    if (mSpeedLabel && mSpeedLabel->visible())
    {
        y += mSpeedLabel->height() + 2 * p;
    }
    for (const auto &tip : mTips)
    {
        y += tip.fWid->height() + 2 * p;
    }
    for (const auto &toast : mToasts)
    {
        const auto w = toast.fWid;
        w->setY(y);
        y += w->height() + 2 * p;
    }
}
