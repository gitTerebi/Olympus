#include "eepisodelostwidget.h"

#include "textures/game-textures.h"

#include "eframedlabel.h"
#include "framed-button.h"
#include "language.h"

void eEpisodeLostWidget::initialize(const eAction& proceedA) {
    const auto res = resolution();
    const auto& intrfc = GameTextures::interface();
    const int iRes = static_cast<int>(res.uiScale());
    const auto& texs = intrfc[iRes];
    setTexture(texs.fDefeatImage);

    const auto defeatLabel = new eFramedLabel(window());
    defeatLabel->setType(eFrameType::message);
    defeatLabel->setText(Language::zeusText(62, 1));
    defeatLabel->setFontSizeXL();
    defeatLabel->fitContent();
    addWidget(defeatLabel);
    defeatLabel->align(Alignment::center);

    const auto proceedButton = new FramedButton(window());
    proceedButton->setRenderBg(true);
    proceedButton->setUnderline(false);
    proceedButton->setText(Language::zeusText(62, 2));
    proceedButton->fitContent();
    addWidget(proceedButton);
    proceedButton->align(Alignment::hcenter);
    proceedButton->setY(2*height()/3);
    proceedButton->setPressAction(proceedA);
}
