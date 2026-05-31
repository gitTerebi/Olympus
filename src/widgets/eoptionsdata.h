#ifndef EOPTIONSDATA_H
#define EOPTIONSDATA_H

#include "widgets/eoptionsmenu.h"

class eMainWindow;
class GameBoard;
class GameWidget;

#include <vector>

std::vector<eOptionsMenu::ePage> getOptionsPages(eMainWindow* const window,
                                                 GameBoard* const board = nullptr,
                                                 GameWidget* const gw = nullptr);

#endif // EOPTIONSDATA_H
