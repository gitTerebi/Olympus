#ifndef EOPTIONSDATA_H
#define EOPTIONSDATA_H

#include "widgets/eoptionsmenu.h"

class eMainWindow;

#include <vector>

std::vector<eOptionsMenu::ePage> getOptionsPages(eMainWindow* const window);

#endif // EOPTIONSDATA_H
