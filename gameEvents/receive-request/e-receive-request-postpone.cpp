#include "e-receive-request-postpone.h"

#include "engine/e-game-board.h"

namespace
{
    constexpr int kInitialRequest = 0;
    constexpr int kHalfwayReminder = 1;
    constexpr int kDeadline = 2;
    constexpr int kFinalDeadline = 3;
    constexpr int kLastDeadline = 4;

}

void scheduleRequestEvent(eFulfillRequestEvent *request, eGameBoard &board,
                          const int step, const eDate &date)
{
    const auto e = e::make_shared<eFulfillRequestEvent>(
        request->cityId(), eGameEventBranch::child, board);
    e->set(*request, step);
    e->initializeDate(date);
    request->addConsequence(e);
}

bool canPostponeRequestStep(const int requestStep)
{
    return requestStep <= kFinalDeadline;
}

bool canPostponeRequestStep(const eFulfillRequestEvent *request)
{
    if (!request)
        return false;
    return canPostponeRequestStep(request->mRequestStep);
}

bool isPostponedRequestStep(const int requestStep)
{
    return requestStep > kDeadline;
}

int displayMonthsForRequestStep(
    const eFulfillRequestEvent *request,
    const int requestStep)
{
    if (requestStep == kInitialRequest || requestStep == kDeadline)
        return request->warningMonths();
    if (requestStep == kLastDeadline)
        return 0;
    return 6;
}

void advanceRequestStep(eFulfillRequestEvent *request, eGameBoard &board)
{
    if (!request)
        return;

    // inc current req step
    const int requestStep = request->mRequestStep + 1;
    request->mRequestStep = requestStep;

    if (requestStep == kDeadline)
    {
        scheduleRequestEvent(request, board, kDeadline, request->mRequestDeadline);
    }
    else if (requestStep == kFinalDeadline)
    {
        auto deadline = board.date();
        deadline.nextMonths(request->warningMonths());
        request->mRequestDeadline = deadline;
        scheduleRequestEvent(request, board, kFinalDeadline, request->mRequestDeadline);
    }
    else if (requestStep == kHalfwayReminder)
    {
        auto reminderDate = board.date();
        reminderDate.nextMonths(6);
        scheduleRequestEvent(request, board, kHalfwayReminder, reminderDate);
    }
    else if (requestStep == kLastDeadline)
    {
        auto deadline = board.date();
        deadline.nextMonths(6);
        request->mRequestDeadline = deadline;
        scheduleRequestEvent(request, board, kLastDeadline, request->mRequestDeadline);
    }
}
