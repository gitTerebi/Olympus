#include "emenuloadingwidget.h"

#include "textures/game-textures.h"
#include "audio/music.h"
#include "audio/sounds.h"
#include "emessages.h"
#include "elanguage.h"
#include "emainwindow.h"

eMenuLoadingWidget::eMenuLoadingWidget(eMainWindow* const window) :
    eLoadingWidget(GameTextures::menuSize(),
                   [window](std::string& text) {
    const auto& sett = window->settings();
        const bool r = GameTextures::loadNextMenu(sett, text);
        if(r) {
            text = "Loading music...";
            eMusic::loadMenu();
            eSounds::loadButtonSound();
            eLanguage::load();
            return true;
        }
        return false;
    }, window, false) {}
