#include "egameloadingwidget.h"

#include "textures/game-textures.h"
#include "audio/music.h"
#include "audio/sounds.h"
#include "messages.h"
#include "buildings/sanctuaries/esanctuaryblueprint.h"
#include "main-window.h"
#include "language.h"
#include "game-dir.h"

eGameLoadingWidget::eGameLoadingWidget(MainWindow* const window) :
    eLoadingWidget(GameTextures::gameSize(window->settings()) + 4,
                   [window](std::string& text) {
        const auto sett = window->settings();
        const bool r = GameTextures::loadNextGame(sett, text);
        if(r) {
            const bool r = eMusic::loaded();
            if(r) {
                const bool r = eSounds::loaded();
                if(r) {
                    const bool r = eSanctBlueprints::loaded();
                    if(r) {
                        const bool r = Messages::loaded();
                        if(r) return true;
                        text = "Loading messages...";
                        Messages::load();
                        return false;
                    }
                    text = "Loading blueprints...";
                    eSanctBlueprints::load();
                    return false;
                } else {
                    text = "Loading sounds...";
                    eSounds::load();
                    return false;
                }
            } else {
                text = "Loading music...";
                eMusic::load();
                return false;
            }
        }
        return false;
    }, window) {}
