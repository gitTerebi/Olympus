#ifndef DEV_LAUNCH_H
#define DEV_LAUNCH_H

#include <string>

class MainWindow;

struct DevLaunchOptions {
    bool fLoadRecent = false;
    bool fCycleDirs = false;
    bool fWorldMap = false;
    std::string fWorldMapShotPath;
};

void applyDevLaunchOptions(MainWindow& window,
                           const DevLaunchOptions& options);

#endif // DEV_LAUNCH_H
