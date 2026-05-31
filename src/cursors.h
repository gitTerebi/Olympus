#ifndef CURSORS_H
#define CURSORS_H

struct SDL_Cursor;

enum class CursorType {
    defaultCursor,
    shovel,
    repairMallet,
    stamp,
    sword
};

class Cursors {
public:
    static bool initialize();
    static void destroy();
    static void set(CursorType type);

private:
    static SDL_Cursor* load(const char* filename, int hotX, int hotY);

    static SDL_Cursor* sDefault;
    static SDL_Cursor* sShovel;
    static SDL_Cursor* sRepairMallet;
    static SDL_Cursor* sStamp;
    static SDL_Cursor* sSword;
};

#endif // CURSORS_H
