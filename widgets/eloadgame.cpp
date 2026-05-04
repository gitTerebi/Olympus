#include "eloadgame.h"

#include "eframedwidget.h"
#include "emainwindow.h"

#include "eacceptbutton.h"
#include "ecancelbutton.h"
#include "escrollbar.h"

#include "elineedit.h"

#include "eframedbutton.h"
#include "equestionwidget.h"

#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

template <typename TP>
time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

void eLoadGame::intialize(const std::string& title,
                            const std::string& folder,
                            const eFileFunc& func,
                            const eAction& closeAction) {
    mFolder = folder;
    mCloseAction = closeAction;

    const auto res = window()->resolution();
    const int ww = res.centralWidgetSmallWidth();
    const int hh = res.centralWidgetSmallHeight();
    ePopupWidget::initialize(ww, hh);

    const auto f = frame();
    const int p = f->padding();

    mTitleLabel = new eLabel(title, window());
    mTitleLabel->fitContent();
    f->addWidget(mTitleLabel);
    mTitleLabel->align(eAlignment::top | eAlignment::hcenter);
    mTitleLabel->setY(mTitleLabel->y() + p);

    mOk = new eAcceptButton(window());
    f->addWidget(mOk);
    mOk->align(eAlignment::bottom | eAlignment::right);
    mOk->move(mOk->x() - 2*p, mOk->y() - 2*p);

    mCancel = new eCancelButton(window());
    f->addWidget(mCancel);
    mCancel->align(eAlignment::bottom | eAlignment::right);
    mCancel->move(mCancel->x() - 2*p - mOk->width() - p, mCancel->y() - 2*p);
    mCancel->setPressAction([this]() { closePopup(); });
    mOk->setPressAction([this, func, closeAction] {
        const auto path = filePath();
        const bool r = func(path);
        if(r) closeAction();
    });

    const auto deleteB = new eFramedButton(window());
    deleteB->setUnderline(false);
    deleteB->setVerySmallFontSize();
    deleteB->setLightFontColor();
    deleteB->setText("Delete");
    deleteB->setTextAlignment(eAlignment::center);
    deleteB->fitContent();
    deleteB->setWidth(90);
    f->addWidget(deleteB);
    deleteB->align(eAlignment::bottom | eAlignment::left);
    deleteB->move(deleteB->x() + 2*p, deleteB->y() - 2*p);
    deleteB->setPressAction([this]() {
        const auto name = mLineEdit->text();
        if(name.empty()) return;
        const auto path = mFolder + name + ".ez";
        if(!std::filesystem::exists(path)) return;
        const auto q = new eQuestionWidget(window());
        const auto acceptA = [this, path]() {
            std::filesystem::remove(path);
            mLineEdit->setText("");
            rebuildFileList();
        };
        std::string msg = "Delete '" + name + "'?";
        q->initialize("Confirm Delete", msg, acceptA, nullptr);
        window()->execDialog(q);
        q->align(eAlignment::center);
    });

    const auto lineW = new eFramedWidget(window());
    lineW->setType(eFrameType::inner);
    lineW->setNoPadding();
    mLineEdit = new eLineEdit(window());
    mLineEdit->setTinyPadding();
    mLineEdit->setText("A");
    mLineEdit->fitContent();
    mLineEdit->setSmallFontSize();
    mLineEdit->setText("");
    lineW->addWidget(mLineEdit);
    mLineEdit->setX(p);
    const int lineY = mTitleLabel->y() + mTitleLabel->height() + p;
    lineW->setY(lineY);
    lineW->setX(2*p);
    f->addWidget(lineW);

    const int vpY = lineY + mLineEdit->height() + 2*p;
    const int vpH = f->height() - vpY - mOk->height() - 4*p;

    const auto sidebar = new eScrollBar(window());
    sidebar->initialize(vpH);
    const int vpW = ww - 4*p - sidebar->width() - p;

    mViewport = new eScrollViewport(window());
    mViewport->setNoPadding();
    mViewport->resize(vpW, vpH);
    mViewport->move(2*p, vpY);
    f->addWidget(mViewport);

    sidebar->move(2*p + vpW + p, vpY);
    f->addWidget(sidebar);
    sidebar->setViewport(mViewport);

    mSwWidth = vpW;
    const int swwidth = mSwWidth;

    mFilesWidget = new eWidget(window());

    std::map<time_t, fs::path> sorted;
    if(std::filesystem::exists(folder)) {
        for(const auto& entry : fs::directory_iterator(folder)) {
            const auto path = entry.path();
            const auto ext = path.extension();
            if(ext != ".ez") continue;
            const auto lwt = fs::last_write_time(path);
            const auto time = to_time_t(lwt);
            sorted[-time] = path;
        }
    }

    int y = 0;
    for(const auto& entry : sorted) {
        const auto path = entry.second;
        const auto name = path.filename().stem().u8string();
        const auto b = new eButton(name, window());
        b->setUnderline(false);
        b->setDarkFontColor();
        b->setMouseEnterAction([b]() {
            b->setLightFontColor();
        });
        b->setMouseLeaveAction([b]() {
            b->setDarkFontColor();
        });
        b->setTextAlignment(eAlignment::left | eAlignment::vcenter);
        b->setNoPadding();
        b->fitContent();
        b->setWidth(swwidth);
        mFilesWidget->addWidget(b);
        b->setY(y);
        y += b->height();
        b->setPressAction([this, name]() {
            setFileName(name);
        });
    }
    mFilesWidget->setNoPadding();
    mFilesWidget->fitContent();

    mFilesWidget->setWidth(mSwWidth);
    mViewport->setPage(mFilesWidget);

    mLineEdit->resize(swwidth - 2*p, mLineEdit->height());
    lineW->resize(swwidth, mLineEdit->height());
}

void eLoadGame::setFileName(const std::string& path) {
    mLineEdit->setText(path);
}

std::string eLoadGame::filePath() const {
    return mFolder + mLineEdit->text() + ".ez";
}

void eLoadGame::rebuildFileList() {
    while (!mFilesWidget->children().empty()) {
        auto w = mFilesWidget->children().back();
        mFilesWidget->removeWidget(w);
        w->deleteLater();
    }
    mFilesWidget->setNoPadding();

    const int swwidth = mSwWidth;

    std::map<time_t, fs::path> sorted;
    if(std::filesystem::exists(mFolder)) {
        for(const auto& entry : fs::directory_iterator(mFolder)) {
            const auto path = entry.path();
            const auto ext = path.extension();
            if(ext != ".ez") continue;
            const auto lwt = fs::last_write_time(path);
            const auto time = to_time_t(lwt);
            sorted[-time] = path;
        }
    }

    int y = 0;
    for(const auto& entry : sorted) {
        const auto path = entry.second;
        const auto name = path.filename().stem().u8string();

        const auto b = new eButton(name, window());
        b->setUnderline(false);
        b->setDarkFontColor();
        b->setMouseEnterAction([b]() {
            b->setLightFontColor();
        });
        b->setMouseLeaveAction([b]() {
            b->setDarkFontColor();
        });
        b->setTextAlignment(eAlignment::left | eAlignment::vcenter);
        b->setNoPadding();
        b->fitContent();
        b->setWidth(swwidth);
        mFilesWidget->addWidget(b);
        b->setY(y);
        b->setX(0);
        b->setPressAction([this, name]() {
            setFileName(name);
        });

        y += b->height();
    }
    mFilesWidget->fitContent();
    mFilesWidget->setWidth(mSwWidth);
    mViewport->setPage(mFilesWidget);
}

void eLoadGame::closePopup() {
    if(mCloseAction) mCloseAction();
    else deleteLater();
}
