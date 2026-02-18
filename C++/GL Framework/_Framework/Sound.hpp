#pragma once
#include "Common/Common.hpp"

class Sound{
public:
    SoLoud::Wav source;

    Sound(){}
    Sound(string file){ load(file); }

    inline void load(string file){ source.load(file.c_str()); }
};

class SoundPlayer{
private:
    static SoLoud::Soloud soloud;
public:
    static void init(){ soloud.init(); }
    static void deinit(){ soloud.deinit(); }

    static void play(Sound& source){ soloud.play(source.source); }
    static void stopAll(){ soloud.stopAll(); }
    static void setGlobalVolume(float volume){ soloud.setGlobalVolume(volume);}
};

SoLoud::Soloud SoundPlayer::soloud;

