#include "emenuloadingwidget.h"

#include "textures/game-textures.h"
#include "audio/music.h"
#include "audio/sounds.h"
#include "messages.h"
#include "language.h"
#include "main-window.h"

eMenuLoadingWidget::eMenuLoadingWidget(MainWindow* const window) :
    eLoadingWidget(GameTextures::menuSize(),
                   [window](std::string& text) {
    const auto& sett = window->settings();
        const bool r = GameTextures::loadNextMenu(sett, text);
        if(r) {
            text = "Loading music...";
            eMusic::loadMenu();
            eSounds::loadButtonSound();
            Language::load();
            return true;
        }
        return false;
    }, window, false) {}
