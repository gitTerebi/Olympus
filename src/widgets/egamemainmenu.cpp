#include "egamemainmenu.h"

#include "framed-button.h"

#include "language.h"

#include "engine/game-board.h"
#include "fileIO/write-stream.h"

void eGameMainMenu::initialize(const eAction& resumeAct,
                               const eAction& saveAct,
                               const eAction& loadAct,
                               const eAction& optionsAct,
                               const eAction& exitAct) {
    setType(eFrameType::message);

    const auto resButt = new FramedButton(window());
    resButt->setUnderline(false);
    resButt->setText(Language::text("resume_game"));
    resButt->fitContent();
    resButt->setPressAction(resumeAct);
    addWidget(resButt);
    resButt->align(Alignment::hcenter);

    const auto saveButt = new FramedButton(window());
    saveButt->setUnderline(false);
    saveButt->setText(Language::zeusText(1, 4));
    saveButt->fitContent();
    saveButt->setPressAction(saveAct);
    addWidget(saveButt);
    saveButt->align(Alignment::hcenter);

    const auto loadButt = new FramedButton(window());
    loadButt->setUnderline(false);
    loadButt->setText(Language::zeusText(1, 3));
    loadButt->fitContent();
    loadButt->setPressAction(loadAct);
    addWidget(loadButt);
    loadButt->align(Alignment::hcenter);

    const auto optionsButt = new FramedButton(window());
    optionsButt->setUnderline(false);
    optionsButt->setText("Options");
    optionsButt->fitContent();
    optionsButt->setPressAction(optionsAct);
    addWidget(optionsButt);
    optionsButt->align(Alignment::hcenter);

    const auto exitButt = new FramedButton(window());
    exitButt->setUnderline(false);
    exitButt->setText(Language::zeusText(1, 5));
    exitButt->fitContent();
    exitButt->setPressAction(exitAct);
    addWidget(exitButt);
    exitButt->align(Alignment::hcenter);

    layoutVertically();
}
