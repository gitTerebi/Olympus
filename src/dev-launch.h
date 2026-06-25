#ifndef DEV_LAUNCH_H
#define DEV_LAUNCH_H

class MainWindow;

struct DevLaunchOptions {
    bool fLoadRecent = false;
    bool fCycleDirs = false;
};

void applyDevLaunchOptions(MainWindow& window,
                           const DevLaunchOptions& options);

#endif // DEV_LAUNCH_H
