#include "eworldmapwidget.h"

#include "textures/game-textures.h"
#include "textures/interface-textures.h"

#include "engine/game-board.h"
#include "gameEvents/conquest/army-return-event.h"
#include "gameEvents/invasions/invasion-event.h"
#include "gameEvents/requests/troops-sent-event.h"
#include "gameEvents/conquest/player-conquest-event.h"
#include "gameEvents/ereinforcementsevent.h"
#include "gameEvents/conquest/player-raid-event.h"
#include "language.h"
#include "audio/sounds.h"
#include "string-helpers.h"
#include "vector-helpers.h"
#include "numbers.h"

eWorldMapWidget::eWorldMapWidget(MainWindow* const window) :
    eLabel(window) {}

void eWorldMapWidget::setSelectColonyMode(
        const bool scm, const eColonySelection& s) {
    mSelectColonyMode = scm;
    mColonySelection = s;
}

void eWorldMapWidget::setBoard(GameBoard* const b) {
    mGameBoard = b;
    setWorldBoard(b ? &b->world() : nullptr);
}

void eWorldMapWidget::setWorldBoard(WorldBoard* const b) {
    mWorldBoard = b;
    if(b) setMap(b->map());
    updateWidgets();
}

void eWorldMapWidget::setSelectCityAction(const eSelectCityAction& s) {
    mSelectCityAction = s;
}

void eWorldMapWidget::setSetTextAction(const eSetTextAction& s) {
    mSetTextAction = s;
}

enum class eArmyReason {
    raid, conquest, help, home
};

struct eMapArmy {
    stdsptr<WorldCity> fOrigin;

    stdsptr<WorldCity> fTravelFrom;
    stdsptr<WorldCity> fTravelTo;
    double fTravelFrac;

    int fArmySize; // 0, 1, 2, 3
    std::vector<eHeroType> fHeroes;

    eArmyReason fReason;
};

std::vector<eMapArmy> getArmies(GameBoard& board) {
    std::vector<eMapArmy> result;

    const auto& world = board.world();
    const auto date = board.date();
    const auto& cs = board.armyEvents();
    for(const auto c : cs) {
        const auto& forces = c->forces();
        const auto splitForces = forces.splitIntoCities();

        eArmyReason reason = eArmyReason::conquest;

        const auto cDate = c->nextDate();
        const int days = cDate - date;
        const int totDays = dynamic_cast<eReinforcementsEvent*>(c) ?
                            Numbers::sReinforcementsTravelTime :
                            Numbers::sArmyTravelTime;
        const double frac = std::clamp(1. - (1.*days)/totDays, 0., 1.);
        bool reverse = false;
        if(dynamic_cast<ArmyReturnEvent*>(c)) {
            reverse = true;
            reason = eArmyReason::home;
        } else if(dynamic_cast<PlayerRaidEvent*>(c)) {
            reason = eArmyReason::raid;
        } else if(dynamic_cast<TroopsSentEvent*>(c)) {
            reason = eArmyReason::help;
        }
        stdsptr<WorldCity> toCity;
        if(dynamic_cast<eReinforcementsEvent*>(c)) {
            reason = eArmyReason::help;
            const auto cid = c->cityId();
            toCity = world.cityWithId(cid);
        } else {
            toCity = c->city();
        }

        for(const auto& f : splitForces) {
            const auto& forces = f.second;
            const auto cid = f.first;
            const auto fromC = world.cityWithId(cid);
            auto& army = result.emplace_back();
            army.fReason = reason;
            army.fOrigin = world.cityWithId(cid);
            army.fTravelFrac = frac;

            if(reverse) {
                army.fTravelFrom = toCity;
                army.fTravelTo = fromC;
            } else {
                army.fTravelFrom = fromC;
                army.fTravelTo = toCity;
            }
            const int s = forces.fSoldiers.size();
            if(s == 0) {
                army.fArmySize = 0;
            } else {
                army.fArmySize = 1 + std::clamp(s/3, 0, 2);
            }

            for(const auto& h : forces.fHeroes) {
                army.fHeroes.push_back(h.second);
            }
        }

        for(const auto& a : forces.fAllies) {
            auto& army = result.emplace_back();
            army.fReason = reason;
            army.fOrigin = a;
            if(reverse) {
                army.fTravelFrom = toCity;
                army.fTravelTo = a;
            } else {
                army.fTravelFrom = a;
                army.fTravelTo = toCity;
            }
            const int s = a->shields();
            army.fArmySize = 1 + std::clamp(s/2, 0, 2);
            army.fTravelFrac = frac;
        }
    }

    const auto& is = board.invasions();
    for(const auto i : is) {
        if(i->conquestEvent()) continue;
        if(!i->warned()) continue;
        const auto toCid = i->cityId();
        const auto toC = world.cityWithId(toCid);
        const auto sDate = i->nextDate();
        const auto wDate = i->firstWarning();
        const int days = sDate - date;
        const int totDays = sDate - wDate;
        const double frac = std::clamp(1. - (1.*days)/totDays, 0., 1.);
        const auto& cc = i->city();

        auto& army = result.emplace_back();
        army.fOrigin = cc;
        army.fTravelFrac = frac;
        const int s = cc->shields();
        army.fArmySize = 1 + std::clamp(s/2, 0, 2);
        army.fTravelFrom = cc;
        army.fTravelTo = toC;
        army.fReason = eArmyReason::conquest;
    }

    return result;
}

void eWorldMapWidget::paintEvent(ePainter& p) {
    mFrame++;
    eLabel::paintEvent(p);
    if(!mWorldBoard) return;

    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const int iRes = static_cast<int>(res.uiScale());
    const auto& texs = intrfc[iRes];
    const bool editor = mWorldBoard->editorMode();

    const auto& regions = mWorldBoard->regions();
    for(const auto& r : regions) {
        const auto name = r.getName();
        const auto nameFind = mNames.find(name);
        stdsptr<Texture> nameTex;
        if(nameFind == mNames.end()) {
            nameTex = std::make_shared<Texture>();
            const auto res = resolution();
            const int fontSize = res.fontSizeS();
            const auto font = eFonts::defaultFont(fontSize);
            nameTex->loadText(renderer(), name, FontColor::region, *font);
            mNames[name] = nameTex;
        } else {
            nameTex = nameFind->second;
        }
        const int x = r.fX*width();
        const int y = r.fY*height();
        const int dx = x; // - nameTex->width()/2;
        const int dy = y - nameTex->height()/2;
        p.drawTexture(dx, dy, nameTex);
    }

    const auto handleCity = [&](const stdsptr<WorldCity>& ct) {
        const bool v = cityVisible(ct, editor);
        if(!v) return;
        const bool atlantean = ct->nationality() == eNationality::atlantean;
        const auto t = ct->type();
        stdsptr<Texture> tex;
        switch(t) {
        case eCityType::parentCity: {
            if(atlantean) tex = texs.fPoseidonMainCity;
            else tex = texs.fZeusMainCity;
        } break;
        case eCityType::colony: {
            const bool active = ct->active();
            if(active) {
                if(atlantean) tex = texs.fPoseidonCollony;
                else tex = texs.fZeusCollony;
            } else {
                if(atlantean) tex = texs.fPoseidonDisabledCollony;
                else tex = texs.fZeusDisabledCollony;
            }
        } break;
        case eCityType::foreignCity: {
            const auto nat = ct->nationality();
            switch(nat) {
            case eNationality::greek:
                tex = texs.fZeusGreekCity;
                break;
            case eNationality::trojan:
                tex = texs.fZeusTrojanCity;
                break;
            case eNationality::persian:
                tex = texs.fZeusPersianCity;
                break;
            case eNationality::centaur:
                tex = texs.fZeusCentaurCity;
                break;
            case eNationality::amazon:
                tex = texs.fZeusAmazonCity;
                break;

            case eNationality::egyptian:
                tex = texs.fPoseidonEgyptianCity;
                break;
            case eNationality::mayan:
                tex = texs.fPoseidonMayanCity;
                break;
            case eNationality::phoenician:
                tex = texs.fPoseidonPhoenicianCity;
                break;
            case eNationality::oceanid:
                tex = texs.fPoseidonOceanidCity;
                break;
            case eNationality::atlantean:
                tex = texs.fPoseidonAtlanteanCity;
                break;
            }
        } break;

        case eCityType::enchantedPlace:
            tex = texs.fZeusPlace;
            break;
        case eCityType::destroyedCity:
            tex = texs.fZeusRuins;
            break;

        case eCityType::distantCity: {
            const auto dir = ct->direction();
            switch(dir) {
            case eDistantDirection::none:
                tex = texs.fZeusDistantCity;
                break;
            case eDistantDirection::N:
                tex = texs.fZeusDistantCityN;
                break;
            case eDistantDirection::NE:
                tex = texs.fZeusDistantCityNE;
                break;
            case eDistantDirection::E:
                tex = texs.fZeusDistantCityE;
                break;
            case eDistantDirection::SE:
                tex = texs.fZeusDistantCitySE;
                break;
            case eDistantDirection::S:
                tex = texs.fZeusDistantCityS;
                break;
            case eDistantDirection::SW:
                tex = texs.fZeusDistantCitySW;
                break;
            case eDistantDirection::W:
                tex = texs.fZeusDistantCityW;
                break;
            case eDistantDirection::NW:
                tex = texs.fZeusDistantCityNW;
                break;
            }
        } break;

        }

        const int x = ct->x()*width();
        const int y = ct->y()*height();
        p.drawTexture(x, y, tex, Alignment::center);

        const auto flagAl = Alignment::hcenter | Alignment::top;
        const int flagX = x + tex->width()/2;
        const int flagY = y + tex->height()/2;
        if(ct->isParentCity()) {
            p.drawTexture(flagX, flagY, texs.fMainCityFlag, flagAl);
        } else if(ct->isVassal() || (ct->isColony() && ct->active())) {
            p.drawTexture(flagX, flagY, texs.fEmpireCityFlag, flagAl);
        } else if(ct->isAlly()) {
            const auto& coll = texs.fAllyCityFlag;
            const int cs = coll.size();
            const auto& tex = coll.getTexture(mFrame % cs);
            p.drawTexture(flagX, flagY, tex, flagAl);
        }

        const auto hc = mWorldBoard->currentCity();
        if(ct != hc &&
           t != eCityType::destroyedCity &&
           t != eCityType::distantCity &&
           t != eCityType::enchantedPlace &&
           (editor || ct->active())) {
            const auto& aColl = texs.fCityArmy;
            const auto& wColl = texs.fCityWealth;
            const int s = editor ? ct->militaryStrength() :
                                   ct->shields();
            const int a = std::clamp(s, 1, 5);
            const int w = std::clamp(ct->wealth(), 1, 5);
            const int lp = res.paddingL();
            const int hp = res.paddingXL();
            int xx = x - hp;
            const int yy = y - hp;
            const auto& aTex = aColl.getTexture(a - 1);
            p.drawTexture(xx, yy, aTex, Alignment::top);
            xx += lp + aTex->width()/2;
            const auto& wTex = wColl.getTexture(w - 1);
            p.drawTexture(xx, yy, wTex, Alignment::top);
        }

        if(ct->rebellion() || ct->conqueredByRival()) {
            const auto& coll = texs.fCityRebellion;
            const int cs = coll.size();
            const auto tex = coll.getTexture(mFrame % cs);
            p.drawTexture(x, y, tex);
        }

        {
            const auto& name = ct->name();
            const auto nameFind = mNames.find(name);
            stdsptr<Texture> nameTex;
            if(nameFind == mNames.end()) {
                nameTex = std::make_shared<Texture>();
                const auto res = resolution();
                const int fontSize = res.fontSizeS();
                const auto font = eFonts::defaultFont(fontSize);
                nameTex->loadText(renderer(), name, FontColor::light, *font);
                mNames[name] = nameTex;
            } else {
                nameTex = nameFind->second;
            }
            int dx = x;
            int dy = y;
            const auto place = ct->namePlace();
            switch(place) {
            case eNamePlace::left:
                dx -= tex->width()/2 + nameTex->width();
                dy -= nameTex->height()/2;
                break;
            case eNamePlace::top:
                dx -= nameTex->width()/2;
                dy -= (tex->height() + nameTex->height())/2;
                break;
            case eNamePlace::right:
                dx += tex->width()/2;
                dy -= nameTex->height()/2;
                break;
            case eNamePlace::bottom:
                dx -= nameTex->width()/2;
                dy += tex->height()/2;
                break;
            }

            p.drawTexture(dx, dy, nameTex);
        }
    };

    const auto hc = mWorldBoard->currentCity();
    const auto& cts = mWorldBoard->cities();
    for(const auto& ct : cts) {
        handleCity(ct);
    }

    const auto cityFigures = [&](const eNationality nat) {
        switch(nat) {
        case eNationality::greek:
            return &texs.fZeusGreekArmy;
        case eNationality::trojan:
            return &texs.fZeusTrojanArmy;
        case eNationality::persian:
            return &texs.fZeusPersianArmy;
        case eNationality::centaur:
            return &texs.fZeusCentaurArmy;
        case eNationality::amazon:
            return &texs.fZeusAmazonArmy;

        case eNationality::egyptian:
            return &texs.fPoseidonEgyptianArmy;
        case eNationality::mayan:
            return &texs.fPoseidonMayanArmy;
        case eNationality::phoenician:
            return &texs.fPoseidonPhoenicianArmy;
        case eNationality::oceanid:
            return &texs.fPoseidonOceanidArmy;
        case eNationality::atlantean:
            return &texs.fPoseidonAtlanteanArmy;
        default:
            return static_cast<const TextureCollection*>(nullptr);
        }
        return static_cast<const TextureCollection*>(nullptr);
    };

    if(mGameBoard) {
        const auto armies = getArmies(*mGameBoard);
        for(const auto& army : armies) {
            int x;
            int y;
            armyDrawXY(*army.fTravelFrom, *army.fTravelTo, army.fTravelFrac, x, y);
            const int dx = res.paddingL();
            if(army.fArmySize != 0) {
                const int n = std::clamp(army.fArmySize - 1, 0, 2);
                const TextureCollection* coll = nullptr;
                const auto& origin = army.fOrigin;
                const auto nat = origin->nationality();
                const auto ppid = mGameBoard->personPlayer();
                const auto pid = army.fOrigin->playerId();
                if(pid == ppid) {
                    const bool atlantean = nat == eNationality::atlantean;
                    if(atlantean) coll = &texs.fPoseidonPlayerArmy;
                    else coll = &texs.fZeusPlayerArmy;
                } else {
                    coll = cityFigures(nat);
                }
                const auto& tex = coll->getTexture(n);
                p.drawTexture(x, y, tex, Alignment::center);
                x += dx;
            }

            for(const auto h : army.fHeroes) {
                stdsptr<Texture> tex;
                switch(h) {
                case eHeroType::achilles:
                    tex = texs.fZeusAchilles;
                    break;
                case eHeroType::atalanta:
                    tex = texs.fPoseidonAtalanta;
                    break;
                case eHeroType::bellerophon:
                    tex = texs.fPoseidonBellerophon;
                    break;
                case eHeroType::hercules:
                    tex = texs.fZeusHercules;
                    break;
                case eHeroType::jason:
                    tex = texs.fZeusJason;
                    break;
                case eHeroType::odysseus:
                    tex = texs.fZeusOdysseus;
                    break;
                case eHeroType::perseus:
                    tex = texs.fZeusPerseus;
                    break;
                case eHeroType::theseus:
                    tex = texs.fZeusTheseus;
                    break;
                }

                p.drawTexture(x, y, tex, Alignment::center);
                x += dx;
            }
        }
    }
}

bool eWorldMapWidget::mousePressEvent(const eMouseEvent& e) {
    if(e.button() == eMouseButton::right) {
        if(mSelectCityAction) mSelectCityAction(nullptr);
        return true;
    } else {
        return false;
    }
}

class eTransparentWidget : public eWidget {
public:
    using eWidget::eWidget;

    void setPressAction(const eAction& a) {
        mPressAction = a;
    }

    void setReleaseAction(const eAction& a) {
        mReleaseAction = a;
    }
protected:
    bool mousePressEvent(const eMouseEvent& e) {
        if(e.button() == eMouseButton::left) {
            if(mPressAction) mPressAction();
            eSounds::playButtonSound();
            return true;
        } else {
            return false;
        }
    }

    bool mouseReleaseEvent(const eMouseEvent& e) {
        if(e.button() == eMouseButton::left) {
            if(mReleaseAction) mReleaseAction();
            return true;
        } else {
            return false;
        }
    }

    bool mouseMoveEvent(const eMouseEvent& e) {
        (void)e;
        return true;
    }

    bool mouseEnterEvent(const eMouseEvent& e) {
        (void)e;
        return true;
    }

    bool mouseLeaveEvent(const eMouseEvent& e) {
        (void)e;
        return true;
    }
private:
    eAction mPressAction;
    eAction mReleaseAction;
};

class eCityTransparentWidget : public eTransparentWidget {
public:
    using eTransparentWidget::eTransparentWidget;

    void setCity(const stdsptr<WorldCity>& c) {
        mCity = c;
    }
protected:
    bool mousePressEvent(const eMouseEvent& e) {
        if(e.button() == eMouseButton::left) {
            mPressX = e.x();
            mPressY = e.y();
        }
        return eTransparentWidget::mousePressEvent(e);
    }

    bool mouseMoveEvent(const eMouseEvent& e) {
        if(static_cast<bool>(e.buttons() & eMouseButton::left)) {
            if(!mCity) return true;
            const auto p = parent();
            const double pw = p->width();
            const double ph = p->height();
            const double cx = (x() + width()/2 + e.x() - mPressX)/pw;
            const double cy = (y() + height()/2 + e.y() - mPressY)/ph;
            mCity->move(cx, cy);
        }
        return true;
    }
private:
    stdsptr<WorldCity> mCity;

    int mPressX = 0;
    int mPressY = 0;
};

void eWorldMapWidget::updateWidgets() {
    removeAllWidgets();
    if(!mWorldBoard) return;
    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const int iRes = static_cast<int>(res.uiScale());
    const auto& texs = intrfc[iRes];
    const auto& tex = texs.fZeusMainCity;
    const int w = tex->width();
    const int h = tex->height();
    const int w2 = 2*w;
    const int h2 = 2*h;

    const int group = 44;
    const int string = 334;
    const auto clickForInfo = Language::zeusText(group, string);

    const bool editor = mWorldBoard->editorMode();

    if(mGameBoard) {
        const auto armies = getArmies(*mGameBoard);
        for(const auto& army : armies) {
            {
                int cx;
                int cy;
                armyDrawXY(*army.fTravelFrom, *army.fTravelTo,
                           army.fTravelFrac, cx, cy);
                const int x = cx - w2/2;
                const int y = cy - h2/2;
                const auto ww = new eTransparentWidget(window());
                ww->setPressAction([this, army]() {
                    if(mSetTextAction) {
                        int string = 15;
                        const auto pid = army.fOrigin->playerId();
                        const auto ppid = mGameBoard->personPlayer();
                        if(pid == ppid) {
                            switch(army.fReason) {
                            case eArmyReason::conquest:
                                string = 11;
                                break;
                            case eArmyReason::help:
                                string = 9;
                                break;
                            case eArmyReason::home:
                                string = 15;
                                break;
                            case eArmyReason::raid:
                                string = 13;
                                break;
                            }
                        } else if(army.fOrigin->isRival()) {
                            string = 25;
                        } else { // ally, colony, vassal
                            switch(army.fReason) {
                            case eArmyReason::conquest:
                                string = 19;
                                break;
                            case eArmyReason::help:
                                string = 23;
                                break;
                            case eArmyReason::home:
                                string = 23;
                                break;
                            case eArmyReason::raid:
                                string = 21;
                                break;
                            }
                        }
                        auto text = Language::zeusText(47, string);
                        const auto targetName = army.fTravelTo->name();
                        StringHelpers::replace(text, "[city_name]", targetName);
                        StringHelpers::replace(text, "[rival_city_name]", targetName);
                        mSetTextAction(text);
                    }
                });
                ww->resize(w2, h2);
                addWidget(ww);
                ww->move(x, y);
                ww->setTooltip(clickForInfo);
            }
        }
    }

    const auto& cts = mWorldBoard->cities();
    for(const auto& ct : cts) {
        const bool v = cityVisible(ct, editor);
        if(!v) continue;
        const int cx = width()*ct->x();
        const int cy = height()*ct->y();
        const int x = cx - w/2;
        const int y = cy - h/2;
        const auto ww = new eCityTransparentWidget(window());
        if(editor) {
            ww->setCity(ct);
            ww->setReleaseAction([this]() {
                updateWidgets();
            });
        }
        ww->setPressAction([this, ct]() {
            if(mSelectColonyMode) {
                for(const auto& c : mColonySelection) {
                    c->setState(eCityState::inactive);
                }
                ct->setState(eCityState::active);
            }
            if(mSelectCityAction) mSelectCityAction(ct);
        });
        ww->resize(w, h);
        addWidget(ww);
        ww->move(x, y);
        ww->setTooltip(clickForInfo);
    }
}

void eWorldMapWidget::armyDrawXY(WorldCity& c1, WorldCity& c2,
                                 const double frac, int& x, int& y) {
    const double hx = c1.x();
    const double hy = c1.y();
    const double ccx = c2.x();
    const double ccy = c2.y();
    x = (hx + (ccx - hx)*frac)*width();
    y = (hy + (ccy - hy)*frac)*height();
}

bool eWorldMapWidget::cityVisible(const stdsptr<WorldCity>& c,
                                  const bool editor) const {
    if(!editor && !c->visible()) return false;
    if(!mSelectColonyMode && !editor &&
       !c->active() && !c->isOnBoard()) return false;
    if(mSelectColonyMode) {
        const bool cc = VectorHelpers::contains(mColonySelection, c);
        if(!cc) return false;
    }
    return true;
}

void eWorldMapWidget::setMap(const eWorldMap map) {
    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const int iRes = static_cast<int>(res.uiScale());
    const auto& texs = intrfc[iRes];

    stdsptr<Texture> tex;
    switch(map) {
    case eWorldMap::greece1:
        GameTextures::loadMapOfGreece1();
        tex = texs.fMapOfGreece1;
        break;
    case eWorldMap::greece2:
        GameTextures::loadMapOfGreece2();
        tex = texs.fMapOfGreece2;
        break;
    case eWorldMap::greece3:
        GameTextures::loadMapOfGreece3();
        tex = texs.fMapOfGreece3;
        break;
    case eWorldMap::greece4:
        GameTextures::loadMapOfGreece4();
        tex = texs.fMapOfGreece4;
        break;
    case eWorldMap::greece5:
        GameTextures::loadMapOfGreece5();
        tex = texs.fMapOfGreece5;
        break;
    case eWorldMap::greece6:
        GameTextures::loadMapOfGreece6();
        tex = texs.fMapOfGreece6;
        break;
    case eWorldMap::greece7:
        GameTextures::loadMapOfGreece7();
        tex = texs.fMapOfGreece7;
        break;
    case eWorldMap::greece8:
        GameTextures::loadMapOfGreece8();
        tex = texs.fMapOfGreece8;
        break;

    case eWorldMap::poseidon1:
        GameTextures::loadPoseidonMap1();
        tex = texs.fPoseidonMap1;
        break;
    case eWorldMap::poseidon2:
        GameTextures::loadPoseidonMap2();
        tex = texs.fPoseidonMap2;
        break;
    case eWorldMap::poseidon3:
        GameTextures::loadPoseidonMap3();
        tex = texs.fPoseidonMap3;
        break;
    case eWorldMap::poseidon4:
        GameTextures::loadPoseidonMap4();
        tex = texs.fPoseidonMap4;
        break;
    }

    setTexture(tex);
    fitContent();
}
