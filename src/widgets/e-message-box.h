#ifndef EMESSAGEBOX_H
#define EMESSAGEBOX_H

#include "eframedwidget.h"

#include "message.h"
#include "engine/edate.h"
#include "engine/ecityid.h"

class WorldCity;
struct eEventData;
enum class eResourceType;
class eLabel;
class GameWidget;

class eMessageBox : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(GameBoard &board,
                    const eEventData& ed,
                    const eAction& viewTile,
                    const eAction& closeFunc,
                    Message msg);

    void close();
    bool closable() const { return mClosable; }
    bool actionTaken() const { return mActionTaken; }

    eWidget* createTributeWidget(const eResourceType type,
                                 const int count, const int space,
                                 const int months = -1,
                                 eLabel** spaceLabelPtr = nullptr,
                                 GameBoard* board = nullptr,
                                 const eCityId cid = eCityId::neutralAggresive,
                                 eLabel** stockLabelPtr = nullptr);
protected:
//    void paintEvent(ePainter& p);
    bool keyPressEvent(const eKeyPressEvent& e);
    bool mousePressEvent(const eMouseEvent& e);
    bool mouseReleaseEvent(const eMouseEvent& e);
private:
    bool mClosable = false;
    bool mActionTaken = false;
    eAction mCloseFunc;
//    eAction mDone;
};

#endif // EMESSAGEBOX_H
