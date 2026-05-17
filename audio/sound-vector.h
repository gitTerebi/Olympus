#ifndef SOUND_VECTOR_H
#define SOUND_VECTOR_H

#include <string>
#include <vector>

#include <SDL2/SDL_mixer.h>

enum class eSoundType {
    voice,
    event,
    ambient
};

class eSoundVector {
public:
    ~eSoundVector();

    static void setGeneralVolume(const int volume);
    static void setVolume(const eSoundType type, const int volume);
    static void reapplyVolumes();

    int soundCount() const { return mPaths.size(); }
    void addPath(const std::string& path);
    void play(const int id, const int chn = -1);
    void play(const int id, const eSoundType type, const int chn = -1);
    void playRandomSound(const eSoundType type = eSoundType::event);
private:
    std::vector<std::pair<Mix_Chunk*, std::string>> mPaths;
    std::vector<int> mRecent;
    static constexpr int kRecentMax = 3;
};

#endif // SOUND_VECTOR_H
