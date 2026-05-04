#ifndef ECURSORS_H
#define ECURSORS_H

struct SDL_Cursor;

enum class eCursorType {
    defaultCursor,
    shovel,
    repairMallet
};

class eCursors {
public:
    static bool initialize();
    static void destroy();
    static void set(eCursorType type);

private:
    static SDL_Cursor* load(const char* filename, int hotX, int hotY);

    static SDL_Cursor* sDefault;
    static SDL_Cursor* sShovel;
    static SDL_Cursor* sRepairMallet;
};

#endif // ECURSORS_H
