#include "e-message-box.h"

#include "elabel.h"
#include "eframedwidget.h"
#include "eokbutton.h"
#include "eexclamationbutton.h"
#include "eframedbutton.h"

#include <stdexcept>
#include <algorithm>

#include "elanguage.h"

#include "estringhelpers.h"
#include "engine/e-worldcity.h"
#include "widgets/game-widget.h"

#include "engine/e-game-board.h"
#include "emainwindow.h"
#include "eboardcityswitchbutton.h"

#include "engine/egifthelpers.h"

template<typename ... Args>
std::string string_format(const std::string& format, Args... args) {
    const int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if(size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    const auto size = static_cast<size_t>(size_s);
    const auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

void eMessageBox::initialize(eGameBoard& board,
                             const eEventData& ed,
                             const eAction& viewTile,
                             const eAction& closeFunc,
                             eMessage msg) {
    const auto boardPtr = &board;
    mCloseFunc = closeFunc;
    setType(eFrameType::message);

    const int p = padding();

    const auto w0 = new eWidget(window());
    {
        w0->setNoPadding();
        if(const auto& c = ed.fCity) {
            eStringHelpers::replaceAll(msg.fTitle, "[city_name]",
                                       c->name());
        }
        if(const auto& c = ed.fRivalCity) {
            eStringHelpers::replaceAll(msg.fTitle, "[rival_city_name]",
                                       c->name());
        }
        {
            const auto type = ed.fResourceType;
            const auto nameShort = eResourceTypeHelpers::typeName(type);
            eStringHelpers::replaceAll(msg.fTitle, "[itemshort]",
                                       nameShort);
        }

        eStringHelpers::replaceAll(msg.fTitle, "[god]",
                                   eGod::sGodName(ed.fGod));

        eStringHelpers::replaceAll(msg.fTitle, "[monster]",
                                   eMonster::sMonsterName(ed.fMonster));

        const auto title = new eLabel(msg.fTitle, window());
        title->setFontSizeXL();
        title->fitContent();
        w0->addWidget(title);
        w0->fitContent();
        w0->setWidth(width() - 2*p);
        title->align(eAlignment::hcenter);
        addWidget(w0);
    }

    const auto ww = new eFramedWidget(window());
    ww->setType(eFrameType::inner);
    ww->setPaddingS();

    {
        const auto to = eLanguage::zeusText(63, 5); // to
        const auto dateStr = ed.fDate.shortString();
        auto str = dateStr + "     " + to + " " + ed.fPlayerName;
        const auto& target = ed.fTarget;
        if(target.isCityTarget()) {
            const auto cid = target.cityTarget();
            const auto cName = board.cityName(cid);
            str += ",     " + cName;
        }
        const auto d = new eLabel(str, window());
        d->setFontSizeS();
        d->fitContent();
        ww->addWidget(d);
        d->setX(p);
    }

    if(viewTile) {
        const auto www = new eWidget(window());
        www->setNoPadding();
        const auto butt = new eExclamationButton(window());
        butt->setPressAction(viewTile);
        www->addWidget(butt);
        const auto go = eLanguage::zeusText(12, 1); // got to site of event
        const auto l = new eLabel(go, window());
        l->setFontSizeS();
        l->fitContent();
        www->addWidget(l);
        www->stackHorizontally();
        www->fitContent();
        butt->align(eAlignment::vcenter);
        ww->addWidget(www);
        www->setX(3*p);
    }

    const auto text = new eLabel(window());
    text->setFontSizeS();
    text->setWrapWidth(width() - 8*p);
    eStringHelpers::replaceAll(msg.fText, "[greeting]",
                               eLanguage::text("greetings"));
    eStringHelpers::replaceAll(msg.fText, "[player_name]",
                               ed.fPlayerName);
    eStringHelpers::replaceAll(msg.fText, "[god]",
                               eGod::sGodName(ed.fGod));
    eStringHelpers::replaceAll(msg.fText, "[monster]",
                               eMonster::sMonsterName(ed.fMonster));

    const auto type = ed.fResourceType;
    const auto item = eResourceTypeHelpers::typeLongName(type);
    const auto itemshort = eResourceTypeHelpers::typeName(type);
    const int count = ed.fResourceCount;
    const auto countStr = std::to_string(count);

    eStringHelpers::replaceAll(msg.fText, "[amount]",
                               countStr);
    eStringHelpers::replaceAll(msg.fText, "[amount_granted]",
                               countStr);
    eStringHelpers::replaceAll(msg.fText, "[item]",
                               item);
    eStringHelpers::replaceAll(msg.fText, "[itemshort]",
                               itemshort);

    const int giftSize = eGiftHelpers::giftCount(type);
    if(giftSize > 0) {
        const int size = count/giftSize;
        std::string giftSize;
        if(size < 2) giftSize = eLanguage::zeusText(162, 0);
        else if(size < 3) giftSize = eLanguage::zeusText(162, 1);
        else giftSize = eLanguage::zeusText(162, 2);
        eStringHelpers::replaceAll(msg.fText, "[gift_size]",
                                   giftSize);
    }

    if(const auto c = ed.fCity) {
        const auto nat = c->nationality();
        const auto natName = eWorldCity::sNationalityName(nat);
        eStringHelpers::replaceAll(msg.fText, "[nationality]",
                                   natName);
        eStringHelpers::replaceAll(msg.fText, "[city_name]",
                                   c->name());
        eStringHelpers::replaceAll(msg.fText, "[last_colony]",
                                   c->name());
        eStringHelpers::replaceAll(msg.fText, "[leader_name]",
                                   c->leader());
        eStringHelpers::replaceAll(msg.fText, "[a_foreign_army]",
                                   c->anArmy());
        if(ed.fType == eMessageEventType::invasionMessage) {
            eStringHelpers::replaceAll(msg.fText, "[time_until_attack]",
                                      std::to_string(ed.fTime));
        }
    }
    const auto c = ed.fRivalCity ? ed.fRivalCity : ed.fCity;
    if(c) {
        const auto nat = c->nationality();
        const auto natName = eWorldCity::sNationalityName(nat);
        eStringHelpers::replaceAll(msg.fText, "[rival_nationality]",
                                   natName);
        eStringHelpers::replaceAll(msg.fText, "[rival_city_name]",
                                   c->name());
    }

    ww->addWidget(text);
    addWidget(ww);

    eOkButton* ok = nullptr;
    eWidget* wid = nullptr;
    const bool addOk = ed.fCloseResponse < 0 && ed.fCityConditionalResponses.empty() &&
                       ed.fPrimaryResponse < 0 && ed.fSecondaryResponse < 0 &&
                       ed.fTertiaryResponse < 0;
    if(addOk) {
        ok = new eOkButton(window());
        ok->setPressAction([this]() {
            close();
        });
        addWidget(ok);
    }
    if(ed.fType == eMessageEventType::invasion) {
        wid = new eWidget(window());
        wid->setNoPadding();

        const auto surrenderB = new eFramedButton(window());
        surrenderB->setFontSizeS();
        surrenderB->setUnderline(false);
        surrenderB->setText(eLanguage::zeusText(44, 282));
        surrenderB->fitContent();
        wid->addWidget(surrenderB);
        surrenderB->setPressAction([this, boardPtr, ed]() {
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fPrimaryResponse);
            mActionTaken = true;
            close();
        });
        surrenderB->setVisible(ed.fPrimaryResponse >= 0);

        const bool canBribe = ed.fSecondaryResponse >= 0;
        const auto bribeB = canBribe ?
            static_cast<eButton*>(new eFramedButton(window())) :
            new eButton(window());
        bribeB->setFontSizeS();
        bribeB->setUnderline(false);
        auto bribeStr = eLanguage::zeusText(44, 281);
        eStringHelpers::replace(bribeStr, "[bribe_amount]",
                                std::to_string(ed.fBribe));
        bribeB->setText(bribeStr);
        bribeB->fitContent();
        wid->addWidget(bribeB);
        bribeB->setPressAction([this, boardPtr, ed]() {
            if(ed.fSecondaryResponse < 0) return;
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fSecondaryResponse);
            mActionTaken = true;
            close();
        });
        bribeB->setVisible(true);
        const auto bribeAmount = std::to_string(ed.fBribe) + " drachmas";
        bribeB->setTooltip(ed.fSecondaryResponse >= 0 ? "Bribe demanded: " + bribeAmount :
                           "Need " + bribeAmount + " to bribe");

        const auto fightToDefend = new eFramedButton(window());
        fightToDefend->setFontSizeS();
        fightToDefend->setUnderline(false);
        fightToDefend->setText(eLanguage::zeusText(44, 283));
        fightToDefend->fitContent();
        wid->addWidget(fightToDefend);
        fightToDefend->setPressAction([this, boardPtr, ed]() {
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fTertiaryResponse);
            mActionTaken = true;
            close();
        });

        const int w = width() - 4*p;
        wid->setWidth(w);
        wid->layoutHorizontallyWithoutSpaces();
        wid->fitContent();
        wid->setWidth(w);

        surrenderB->align(eAlignment::vcenter);
        bribeB->align(eAlignment::vcenter);
        fightToDefend->align(eAlignment::vcenter);

        addWidget(wid);
    } else if(ed.fType == eMessageEventType::requestTributeGranted) {
        const auto c = ed.fCity;
        if(!c) return;
        eLabel* spaceLabel = nullptr;
        const auto tributeWid = createTributeWidget(type, count, 0,
                                                    -1, &spaceLabel);

        ww->addWidget(tributeWid);
        tributeWid->setX(p);

        wid = new eWidget(window());
        wid->setNoPadding();

            const auto acceptB = new eFramedButton(window());
            acceptB->setFontSizeS();
            acceptB->setUnderline(false);
            acceptB->setText(eLanguage::zeusText(44, 209));
            acceptB->fitContent();
            if(type == eResourceType::drachmas) {
                wid->addWidget(acceptB);
                acceptB->setPressAction([this, boardPtr, ed]() {
                    boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fPrimaryResponse);
                    mActionTaken = true;
                    close();
                });
            } else if(ed.fCityNames.size() == 1) {
                const auto iniC = ed.fCityNames.begin();
                const auto iniCid = iniC->first;
                wid->addWidget(acceptB);
                acceptB->setPressAction([this, boardPtr, ed, iniCid]() {
                    const auto a0 = ed.fCityConditionalResponses.at(iniCid);
                    boardPtr->respondToEvent(ed.fEventRuntimeId, a0, iniCid);
                    mActionTaken = true;
                    close();
                });
                if(spaceLabel) {
                    const int space = ed.fCSpaceCount.at(iniCid);
                    const int c = std::min(space, count);
                    const auto cStr = std::to_string(c);
                    spaceLabel->setText(cStr);
                }
            } else {
                const auto iniC = ed.fCityNames.begin();
                const auto iniCid = iniC->first;
                const auto iniName = iniC->second;

                const auto cityB = new eBoardCitySwitchButton(window());
                cityB->setFontSizeS();
                const auto setCid = [this, boardPtr, ed, acceptB, spaceLabel, count](const eCityId cid) {
                    const int space = ed.fCSpaceCount.at(cid);
                    if(spaceLabel) {
                        const int c = std::min(space, count);
                        const auto cStr = std::to_string(c);
                        spaceLabel->setText(cStr);
                    }
                    acceptB->setVisible(space > 0);
                    acceptB->setPressAction([this, boardPtr, ed, cid]() {
                        const auto a0 = ed.fCityConditionalResponses.at(cid);
                        boardPtr->respondToEvent(ed.fEventRuntimeId, a0, cid);
                        mActionTaken = true;
                        close();
                    });
                };
                cityB->initialize(ed.fCityNames, setCid);
                setCid(iniCid);
                cityB->setCurrentCity(iniCid);

                wid->addWidget(cityB);
                wid->addWidget(acceptB);
            }

            const auto postponeB = new eFramedButton(window());
            postponeB->setFontSizeS();
            postponeB->setUnderline(false);
            postponeB->setText(eLanguage::zeusText(44, 211));
            postponeB->fitContent();
            wid->addWidget(postponeB);
            postponeB->setPressAction([this, boardPtr, ed]() {
                boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fSecondaryResponse);
                mActionTaken = true;
                close();
            });
            postponeB->setVisible(ed.fSecondaryResponse >= 0 && type != eResourceType::drachmas);

            const auto declineB = new eFramedButton(window());
            declineB->setFontSizeS();
            declineB->setUnderline(false);
            declineB->setText(eLanguage::zeusText(44, 210));
            declineB->fitContent();
            wid->addWidget(declineB);
            declineB->setPressAction([this, boardPtr, ed]() {
                boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fTertiaryResponse);
                mActionTaken = true;
                close();
            });

            const int w = width() - 4*p;
            wid->setWidth(w);
            wid->layoutHorizontallyWithoutSpaces();
            wid->fitContent();
            wid->setWidth(w);

            const auto cs = wid->children();
            for(const auto c : cs) {
                c->align(eAlignment::vcenter);
            }

        addWidget(wid);
    } else if(ed.fType == eMessageEventType::resourceGranted) {
        const auto tributeWid = createTributeWidget(type, count, -1);

        ww->addWidget(tributeWid);
        tributeWid->setX(p);
    } else if(ed.fType == eMessageEventType::generalRequestGranted) {
        const auto c = ed.fCity;
        if(!c) return;
        const int time = ed.fTime;
        const auto timeStr = std::to_string(time);
        eLabel* stockLabel = nullptr;
        const auto iniCid = ed.fCityNames.empty() ?
                            static_cast<eCityId>(0) :
                            ed.fCityNames.begin()->first;
        const int space = time > 0 ? 0 : -1;
        const auto tributeWid = createTributeWidget(type, count, space, time,
                                                    nullptr, &board, iniCid, &stockLabel);

        if(time > 0) {
            eStringHelpers::replaceAll(msg.fText, "[time_allotted]",
                                       timeStr);
        }

        ww->addWidget(tributeWid);
        tributeWid->setX(p);

        wid = new eWidget(window());
        wid->setNoPadding();

        const auto a0B = new eFramedButton(window());
        a0B->setFontSizeS();
        a0B->setUnderline(false);
        a0B->setText(eLanguage::zeusText(44, 275));
        a0B->fitContent();

        if(type == eResourceType::drachmas) {
            wid->addWidget(a0B);
            a0B->setPressAction([this, boardPtr, ed]() {
                boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fPrimaryResponse);
                mActionTaken = true;
                close();
            });
            a0B->setVisible(ed.fPrimaryResponse >= 0);
        } else if(ed.fCityNames.size() == 1) {
            const auto iniC = ed.fCityNames.begin();
            const auto iniCid = iniC->first;
            wid->addWidget(a0B);
            a0B->setVisible(ed.fCSpaceCount.at(iniCid) >= ed.fResourceCount);
            a0B->setPressAction([this, boardPtr, ed, iniCid]() {
                const auto a0 = ed.fCityConditionalResponses.at(iniCid);
                boardPtr->respondToEvent(ed.fEventRuntimeId, a0, iniCid);
                mActionTaken = true;
                close();
            });
        } else {
            const auto iniC = ed.fCityNames.begin();
            const auto iniCid = iniC->first;
            const auto iniName = iniC->second;

            const auto cityB = new eBoardCitySwitchButton(window());
            cityB->setFontSizeS();
            const auto setCid = [this, boardPtr, ed, a0B](const eCityId cid) {
                const int count = ed.fCSpaceCount.at(cid);
                a0B->setVisible(count >= ed.fResourceCount);
                a0B->setPressAction([this, boardPtr, ed, cid]() {
                    const auto a0 = ed.fCityConditionalResponses.at(cid);
                    boardPtr->respondToEvent(ed.fEventRuntimeId, a0, cid);
                    mActionTaken = true;
                    close();
                });
            };
            cityB->initialize(ed.fCityNames, setCid);
            setCid(iniCid);
            cityB->setCurrentCity(iniCid);

            wid->addWidget(cityB);
            wid->addWidget(a0B);
        }

        const auto a1B = new eFramedButton(window());
        a1B->setFontSizeS();
        a1B->setUnderline(false);
        a1B->setText(eLanguage::zeusText(44, 211));
        a1B->fitContent();
        wid->addWidget(a1B);
        a1B->setPressAction([this, boardPtr, ed]() {
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fSecondaryResponse);
            mActionTaken = true;
            close();
        });
        a1B->setVisible(ed.fSecondaryResponse >= 0);

        eButton* a2B;
        if(ed.fType == eMessageEventType::generalRequestGranted && ed.fTime == 0) {
            a2B = new eOkButton(window());
        } else {
            a2B = new eFramedButton(window());
            a2B->setFontSizeS();
            a2B->setUnderline(false);
            a2B->setText(eLanguage::zeusText(44, 212));
            a2B->fitContent();
        }
        wid->addWidget(a2B);
        a2B->setPressAction([this, boardPtr, ed]() {
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fTertiaryResponse);
            mActionTaken = true;
            close();
        });

        const int w = width() - 4*p;
        wid->setWidth(w);
        wid->layoutHorizontallyWithoutSpaces();
        wid->fitContent();
        wid->setWidth(w);
        a0B->align(eAlignment::vcenter);
        a1B->align(eAlignment::vcenter);
        a2B->align(eAlignment::vcenter);

        addWidget(wid);
    } else if(ed.fType == eMessageEventType::troopsRequest) {
        const auto c = ed.fCity;
        if(!c) return;
        const int time = ed.fTime;
        const auto timeStr = std::to_string(time);

        eStringHelpers::replaceAll(msg.fText, "[travel_time]",
                                   timeStr);

        wid = new eWidget(window());
        wid->setNoPadding();

        const auto a0B = new eFramedButton(window());
        a0B->setFontSizeS();
        a0B->setUnderline(false);
        a0B->setText(eLanguage::zeusText(44, 275));
        a0B->fitContent();
        wid->addWidget(a0B);
        if(ed.fCloseResponse >= 0) {
            a0B->setPressAction([this, boardPtr, ed]() {
                mActionTaken = true;
                boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fCloseResponse);
                close();
            });
        } else {
            a0B->setPressAction([this, boardPtr, ed]() {
                boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fPrimaryResponse);
                mActionTaken = true;
                close();
            });
        }
        a0B->setVisible(ed.fPrimaryResponse >= 0 || ed.fCloseResponse >= 0);

        const auto a1B = new eFramedButton(window());
        a1B->setFontSizeS();
        a1B->setUnderline(false);
        a1B->setText(eLanguage::zeusText(44, 211));
        a1B->fitContent();
        wid->addWidget(a1B);
        a1B->setPressAction([this, boardPtr, ed]() {
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fSecondaryResponse);
            mActionTaken = true;
            close();
        });
        a1B->setVisible(ed.fSecondaryResponse >= 0);

        const auto a2B = new eFramedButton(window());
        a2B->setFontSizeS();
        a2B->setUnderline(false);
        a2B->setText(eLanguage::zeusText(44, 212));
        a2B->fitContent();
        wid->addWidget(a2B);
        a2B->setPressAction([this, boardPtr, ed]() {
            boardPtr->respondToEvent(ed.fEventRuntimeId, ed.fTertiaryResponse);
            mActionTaken = true;
            close();
        });
        a2B->setVisible(ed.fTertiaryResponse >= 0);

        const int w = width() - 4*p;
        wid->setWidth(w);
        wid->layoutHorizontallyWithoutSpaces();
        wid->fitContent();
        wid->setWidth(w);
        a0B->align(eAlignment::vcenter);
        a1B->align(eAlignment::vcenter);
        a2B->align(eAlignment::vcenter);

        addWidget(wid);
    }

    text->setText(msg.fText);
    text->fitContent();
    text->setX(p);

    ww->stackVertically();
    ww->fitContent();

    stackVertically();
    fitContent();

    if(ok) {
        mClosable = true;
        ok->align(eAlignment::right | eAlignment::bottom);
        ok->setX(ok->x() - 1.5*p);
        ok->setY(ok->y() - 1.5*p);
    }
    if(wid) {
        wid->align(eAlignment::hcenter);
        wid->setY(wid->y() + p/2);
    }
    w0->align(eAlignment::hcenter);
    ww->align(eAlignment::hcenter);
}

void eMessageBox::close() {
    if(mCloseFunc) mCloseFunc();
    deleteLater();
}

eWidget* eMessageBox::createTributeWidget(const eResourceType type,
                                          const int count,
                                          const int space,
                                          const int months,
                                          eLabel** spaceLabelPtr,
                                          eGameBoard* board,
                                          const eCityId cid,
                                          eLabel** stockLabelPtr) {
    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const auto tributeWid = new eWidget(window());
    tributeWid->setNoPadding();

    auto addIcon = [this, uiScale, tributeWid](const eResourceType t) {
        const auto icon = eResourceTypeHelpers::icon(uiScale, t);
        const auto typeIcon = new eLabel(window());
        typeIcon->setTexture(icon);
        typeIcon->setNoPadding();
        typeIcon->fitContent();
        tributeWid->addWidget(typeIcon);
    };

    auto addLabel = [this, tributeWid](const std::string& text) {
        const auto label = new eLabel(window());
        label->setFontSizeS();
        label->setNoPadding();
        label->setText(text);
        label->fitContent();
        tributeWid->addWidget(label);
        return label;
    };

    addIcon(type);
    const auto countStr = std::to_string(count);
    const auto countLabel = addLabel("9999");
    countLabel->setText(countStr);

    if(stockLabelPtr && board) {
        const int stock = board->resourceCount(cid, type);
        const auto stockText = eLanguage::zeusText(44, 278); // in stock
        const auto stockLabel = addLabel("");
        stockLabel->setText("(" + std::to_string(stock) + " " +
                            stockText + ") ");
        *stockLabelPtr = stockLabel;
        stockLabel->align(eAlignment::hcenter);
        stockLabel->fitContent();
    }

    const auto name = eResourceTypeHelpers::typeLongName(type);
    addLabel(" " + name);

    if(months > 0) {
        const auto monthsStr = std::to_string(months);
        const auto m = eLanguage::zeusText(8, 5);
        const auto c = eLanguage::zeusText(12, 2);
        const auto mtc = m + " " + c;
        addLabel("        " + mtc + " " + monthsStr);
    } else if(space != -1 && type != eResourceType::drachmas) {
        const auto spaceStr = std::to_string(std::min(count, space));
        addLabel(" / " + eLanguage::zeusText(130, 6) + " ");
        addIcon(type);
        const auto spaceLabel = addLabel("9999");
        if(spaceLabelPtr) *spaceLabelPtr = spaceLabel;
        spaceLabel->setText(spaceStr);
        const auto spaceName = eResourceTypeHelpers::typeLongName(type);
        addLabel(" " + spaceName);
    }

    tributeWid->stackHorizontally();
    tributeWid->fitContent();
    return tributeWid;
}

//void eMessageBox::paintEvent(ePainter& p) {
//    eFramedWidget::paintEvent(p);
//    if(mDone) mDone();
//}

bool eMessageBox::keyPressEvent(const eKeyPressEvent& e) {
    if(!mClosable) return true;
    const auto k = e.key();
    if(k == SDL_SCANCODE_ESCAPE) {
        close();
    }
    return true;
}

bool eMessageBox::mousePressEvent(const eMouseEvent& e) {
    return true;
}

bool eMessageBox::mouseReleaseEvent(const eMouseEvent& e) {
    if(!mClosable) return true;
    const auto b = e.button();
    if(b == eMouseButton::right) {
        close();
    }
    return true;
}
