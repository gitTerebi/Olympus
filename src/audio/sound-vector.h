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
    // play this sound only once per every n calls (1 = every call).
    // used to thin out very frequent sounds, e.g. disgruntled hits
    void setPlayEveryNth(const int n) { mPlayEveryNth = n < 1 ? 1 : n; }
    void play(const int id, const int chn = -1);
    void play(const int id, const eSoundType type, const int chn = -1);
    void playRandomSound(const eSoundType type = eSoundType::event);
private:
    std::vector<std::pair<Mix_Chunk*, std::string>> mPaths;
    std::vector<int> mRecent;
    static constexpr int kRecentMax = 3;
    int mPlayEveryNth = 1;
    int mPlayCounter = 0;
};

#endif // SOUND_VECTOR_H
