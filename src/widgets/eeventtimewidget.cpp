#include "eeventtimewidget.h"

void eEventTimeWidget::initialize(const eAction& act) {
    mChangeAction = act;
}

void eEventTimeWidget::setEventTime(const eEventTime et) {
    mEventTime = et;
}

eEventTime eEventTimeWidget::eventTime() const {
    return mEventTime;
}
