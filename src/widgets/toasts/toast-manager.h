#pragma once

#include "message.h"
#include "engine/eeventdata.h"
#include "engine/edate.h"

class eWidget;

struct eToast {
    eEventData fEd;
    Message fMsg;
    eWidget* fWid = nullptr;
    int fExpireTick = 0;
    eDate fDate;
    bool fQueued = false;
};
