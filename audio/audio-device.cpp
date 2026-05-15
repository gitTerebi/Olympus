#include "audio-device.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "music.h"
#include "sound-vector.h"

bool ensureAudioDeviceOpen() {
    int freq; Uint16 fmt; int chans;
    if(Mix_QuerySpec(&freq, &fmt, &chans)) return true;

    static Uint32 sLastTry = 0;
    const Uint32 now = SDL_GetTicks();
    if(sLastTry && now - sLastTry < 1000) return false;
    sLastTry = now;

    if(!SDL_WasInit(SDL_INIT_AUDIO)) {
        if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) return false;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;

    eSoundVector::reapplyVolumes();
    eMusic::reapplyVolumes();
    return true;
}
