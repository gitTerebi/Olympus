#pragma once

#include "emessage.h"
#include "engine/eeventdata.h"
#include "engine/edate.h"

class eWidget;

struct eToast {
    eEventData fEd;
    eMessage fMsg;
    eWidget* fWid = nullptr;
    int fExpireFrame = 0;
    eDate fDate;
    bool fQueued = false;
};
