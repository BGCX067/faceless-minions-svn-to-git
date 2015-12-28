#ifndef ICARUS_SOUND_HPP_
#define ICARUS_SOUND_HPP_
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <unordered_map>


class sound
{
private:
    float volume_sfx_;
    float volume_bgm_;
    sf::Music *background_music_;
    sf::Sound *sound_effects_;
    std::unordered_map<std::string, sf::SoundBuffer*> sound_buffers_;
public:
    sound();
    ~sound();
    void set_bgm_volume(float volume);
    void set_sfx_volume(float volume);
    float get_bgm_volume();
    float get_sfx_volume();
    void load_sfx(const std::string& file_path, const std::string& sfx_name);
    void load_bgm(const std::string& file_path, bool play_now, bool loop);
    void play_sfx(const std::string& sfx_name);
    void play_bgm();
    void pause_bgm();
    void stop_bgm();
    bool get_bgm_playing();
    void unload_all();
};

#endif // ICARUS_SOUND_HPP_