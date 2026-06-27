#ifndef EWORLDMAPWIDGET_H
#define EWORLDMAPWIDGET_H

#include "elabel.h"

#include "engine/world-board.h"

class eWorldMapWidget : public eLabel {
public:
    eWorldMapWidget(MainWindow* const window);

    using eColonySelection = std::vector<stdsptr<WorldCity>>;
    void setSelectColonyMode(const bool scm, const eColonySelection& s);

    void setBoard(GameBoard* const b);
    void setWorldBoard(WorldBoard* const b);

    using eSelectCityAction = std::function<void(const stdsptr<WorldCity>&)>;
    void setSelectCityAction(const eSelectCityAction& s);
    using eSetTextAction = std::function<void(const std::string&)>;
    void setSetTextAction(const eSetTextAction& s);

    void updateWidgets();

    void setMap(const eWorldMap map);

    void setIconScale(const double s);
    double iconScale() const { return mIconScale; }
protected:
    void paintEvent(ePainter& p);
    bool mousePressEvent(const eMouseEvent& e);
private:
    void drawScaled(ePainter& p, const int x, const int y,
                    const stdsptr<Texture>& tex,
                    const Alignment align = Alignment::none);

    void armyDrawXY(WorldCity& c1, WorldCity& c2,
                    const double frac, int& x, int& y);

    bool cityVisible(const stdsptr<WorldCity>& c,
                     const bool editor) const;

    bool mSelectColonyMode = false;
    std::vector<stdsptr<WorldCity>> mColonySelection;

    GameBoard* mGameBoard = nullptr;
    WorldBoard* mWorldBoard = nullptr;
    int mFrame = 0;
    double mIconScale = 1.0;

    std::map<std::string, stdsptr<Texture>> mNames;

    eSelectCityAction mSelectCityAction;
    eSetTextAction mSetTextAction;
};

#endif // EWORLDMAPWIDGET_H
