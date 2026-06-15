#include "eeventselectionwidget.h"

#include "main-window.h"

#include "eventwidgets/eeventwidgetbase.h"
#include "gameEvents/egameevent.h"
#include "language.h"

#include "widgets/echoosebutton.h"

#include "engine/game-board.h"

eEventSelectionWidget::eEventSelectionWidget(
        const eGameEventBranch branch,
        MainWindow* const window) :
    eScrollButtonsList(window),
    mBranch(branch) {}

void eEventSelectionWidget::initialize(
        const eCityId cid,
        const eEventsGetter& get,
        const eEventAdder& add,
        const eEventRemover& remove,
        GameBoard& board) {
    eScrollButtonsList::initialize();

    const auto iniEs = get();
    for(const auto& e : iniEs) {
        const auto eStr = e->longDatedName();
        addButton(eStr);
    }

    const auto editEvent = [this](const stdsptr<eGameEvent>& e) {
        const auto settings = new eEventWidgetBase(window());
        settings->resize(width(), height());
        settings->initialize(e);

        window()->execDialog(settings);
        settings->align(Alignment::center);
    };

    setButtonPressEvent([get, editEvent](const int id) {
        const auto es = get();
        const auto& e = es[id];
        editEvent(e);
    });

    const auto boardPtr = &board;

    setButtonCreateEvent([this, cid, add, editEvent, boardPtr]() {
        const std::vector<eGameEventType> types = {
            eGameEventType::godAttack,
            eGameEventType::monsterUnleashed,
            eGameEventType::monsterInvasion,
            eGameEventType::monsterInCity,
            eGameEventType::invasion,
            eGameEventType::sendResourcesToCity,
            eGameEventType::giftFrom,
            eGameEventType::godQuest,
            eGameEventType::militaryChange,
            eGameEventType::economicChange,
            eGameEventType::sendTroops,
            eGameEventType::godDisaster,
            eGameEventType::rivalArmyAway,
            eGameEventType::earthquake,
            eGameEventType::lavaFlow,
            eGameEventType::tidalWave,
            eGameEventType::sinkLand,
            eGameEventType::cityBecomes,
            eGameEventType::tradeShutdowns,
            eGameEventType::tradeOpensUp,
            eGameEventType::supplyChange,
            eGameEventType::demandChange,
            eGameEventType::priceChange,
            eGameEventType::wageChange
        };
        const std::vector<std::string> labels = {
            Language::zeusText(156, 27), // god invasion
            Language::zeusText(182, 1), // monster unleashed
            Language::zeusText(182, 2), // monster invasion
            Language::zeusText(182, 0), // monster in city
            Language::zeusText(156, 2), // invasion
            Language::zeusText(156, 1), // request
            Language::zeusText(156, 23), // gift
            Language::zeusText(156, 4), // quest
            Language::text("military_change_long_name"),
            Language::text("economic_change_long_name"),
            Language::zeusText(290, 6), // military request
            Language::zeusText(35, 13), // god disaster
            Language::zeusText(156, 20), // rival army away
            Language::zeusText(156, 3), // earthquake
            Language::zeusText(156, 24), // lava flow
            Language::zeusText(156, 25), // tidal wave
            Language::zeusText(156, 28), // sink land
            Language::zeusText(290, 35), // city status change
            Language::zeusText(35, 2), // trade shuts down
            Language::zeusText(35, 3), // trade opens up
            Language::text("supply_change_short_name"),
            Language::text("demand_change_short_name"),
            Language::text("price_change_short_name"),
            Language::text("wage_change")
        };
        const auto echoose = new eChooseButton(window());
        const auto act = [this, cid, add, types, labels, editEvent, boardPtr](const int val) {
            const auto type = types[val];
            const auto e = eGameEvent::sCreate(cid, type, mBranch, *boardPtr);

            if(e) {
                e->setIsEpisodeEvent(true);
                const auto boardDate = eDate(1, eMonth::january, -1500);
                const int period = 150;
                const auto date = boardDate + period;
                e->initializeDate(date, period, 1);
                add(e);
                editEvent(e);
                addButton(e->longDatedName());
            }
        };
        echoose->initialize(10, labels, act);

        window()->execDialog(echoose);
        echoose->align(Alignment::center);
    });

    setButtonRemoveEvent([get, remove](const int id) {
        const auto es = get();
        const auto& e = es[id];
        remove(e);
    });
}
