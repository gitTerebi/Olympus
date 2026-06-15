#include "sound-vector.h"

#include "audio-device.h"
#include "rand.h"
#include <filesystem>
#include <algorithm>
#include <SDL2/SDL_timer.h>

namespace {
constexpr int sVoiceTag = 1;
constexpr int sEventTag = 2;
constexpr int sAmbientTag = 3;
constexpr int sChannelCount = 32;

bool sChannelsConfigured = false;
int sGeneralVolume = 100;
int sVoiceVolume = 100;
int sEventVolume = 100;
int sAmbientVolume = 100;

int clampPercent(const int volume) {
    return std::clamp(volume, 0, 100);
}

int toMixerVolume(const int volume) {
    return MIX_MAX_VOLUME*clampPercent(volume)/100;
}

int applyGeneralVolume(const int volume) {
    return toMixerVolume(sGeneralVolume*clampPercent(volume)/100);
}

int tagForType(const eSoundType type) {
    switch(type) {
    case eSoundType::voice:
        return sVoiceTag;
    case eSoundType::event:
        return sEventTag;
    case eSoundType::ambient:
        return sAmbientTag;
    }
    return sEventTag;
}

int volumeForType(const eSoundType type) {
    switch(type) {
    case eSoundType::voice:
        return sVoiceVolume;
    case eSoundType::event:
        return sEventVolume;
    case eSoundType::ambient:
        return sAmbientVolume;
    }
    return sEventVolume;
}

void configureChannels() {
    if(sChannelsConfigured) return;
    sChannelsConfigured = true;
    Mix_AllocateChannels(sChannelCount);
    Mix_GroupChannels(0, 3, sVoiceTag);
    Mix_GroupChannels(4, 15, sEventTag);
    Mix_GroupChannels(16, 31, sAmbientTag);
}

void setGroupVolume(const int tag, const int volume) {
    configureChannels();
    int first = 4;
    int last = 15;
    if(tag == sVoiceTag) {
        first = 0;
        last = 3;
    } else if(tag == sAmbientTag) {
        first = 16;
        last = 31;
    }
    for(int c = first; c <= last; c++) {
        Mix_Volume(c, volume);
    }
}
}

void eSoundVector::reapplyVolumes() {
    sChannelsConfigured = false;
    configureChannels();
    setGroupVolume(sVoiceTag, applyGeneralVolume(sVoiceVolume));
    setGroupVolume(sEventTag, applyGeneralVolume(sEventVolume));
    setGroupVolume(sAmbientTag, applyGeneralVolume(sAmbientVolume));
}

Mix_Chunk* loadSound(const std::string& path) {
    if(!ensureAudioDeviceOpen()) return nullptr;
    const auto wav = Mix_LoadWAV(path.c_str());
    if(!wav) {
        printf("Failed to load sound '%s'!\n SDL_mixer Error: %s\n",
               path.c_str(), Mix_GetError());
        return nullptr;
    }
    return wav;
}

eSoundVector::~eSoundVector() {
    for(const auto& s : mPaths) {
        if(!s.first) continue;
        Mix_FreeChunk(s.first);
    }
}

const bool sLoadOnAdd = false;

void eSoundVector::setGeneralVolume(const int volume) {
    sGeneralVolume = clampPercent(volume);
    setGroupVolume(sVoiceTag, applyGeneralVolume(sVoiceVolume));
    setGroupVolume(sEventTag, applyGeneralVolume(sEventVolume));
    setGroupVolume(sAmbientTag, applyGeneralVolume(sAmbientVolume));
}

void eSoundVector::setVolume(const eSoundType type, const int volume) {
    const int v = clampPercent(volume);
    switch(type) {
    case eSoundType::voice:
        sVoiceVolume = v;
        break;
    case eSoundType::event:
        sEventVolume = v;
        break;
    case eSoundType::ambient:
        sAmbientVolume = v;
        break;
    }
    setGroupVolume(tagForType(type), applyGeneralVolume(v));
}

void eSoundVector::addPath(const std::string& path) {
    const bool e = std::filesystem::exists(path);
    if(!e) printf("Missing audio file %s\n", path.c_str());
    const auto sound = sLoadOnAdd ? loadSound(path) : nullptr;
    mPaths.push_back({sound, path});
}

void eSoundVector::play(const int id, const int chn) {
    play(id, eSoundType::event, chn);
}

void eSoundVector::play(const int id, const eSoundType type, const int chn) {
    const int idMax = mPaths.size();
    if(id < 0 || id >= idMax) return;
    auto& p = mPaths[id];
    if(!p.first) p.first = loadSound(p.second);
    if(!p.first) return;

    configureChannels();
    int channel = chn;
    const int tag = tagForType(type);
    if(channel < 0) {
        channel = Mix_GroupAvailable(tag);
        if(channel < 0) channel = Mix_GroupOldest(tag);
    }
    if(channel < 0) return;

    Mix_Volume(channel, applyGeneralVolume(volumeForType(type)));
    Mix_PlayChannel(channel, p.first, 0);
}

void eSoundVector::playRandomSound(const eSoundType type) {
    const int sc = soundCount();
    if(sc <= 0) return;
    if(mPlayEveryNth > 1) {
        if(++mPlayCounter < mPlayEveryNth) return;
        mPlayCounter = 0;
    }
    int id = Rand::rand() % sc;
    if(sc > kRecentMax) {
        int tries = 0;
        while(std::find(mRecent.begin(), mRecent.end(), id) != mRecent.end() && tries++ < 8) {
            id = Rand::rand() % sc;
        }
    }
    mRecent.push_back(id);
    if(static_cast<int>(mRecent.size()) > kRecentMax) mRecent.erase(mRecent.begin());
    play(id, type);
}
