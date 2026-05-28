#ifndef EOPTIONSDATA_H
#define EOPTIONSDATA_H

#include "widgets/eoptionsmenu.h"

class eMainWindow;
class eGameBoard;
class eGameWidget;

#include <vector>

std::vector<eOptionsMenu::ePage> getOptionsPages(eMainWindow* const window,
                                                 eGameBoard* const board = nullptr,
                                                 eGameWidget* const gw = nullptr);

#endif // EOPTIONSDATA_H
