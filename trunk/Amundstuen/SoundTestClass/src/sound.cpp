#include <iostream>
#include <string>

#include "../include/sound.hpp"

sound::sound()
{
    //std::cout << "MUSIC" << std::endl;
    volume_sfx_ = 100.0f;
    volume_bgm_ = 100.0f;
    background_music_ = NULL;
    sound::load_sfx("sounds/ball.wav", "ball.wav");
    sound::play_sfx("ball.wav");
    sound::load_bgm("music/money_rules_the_world.ogg", true, true);
    sound::play_bgm();
}

sound::~sound()
{
    /*if (background_music_ != NULL)
    {
        delete background_music_;
    }
    for (unsigned i = 0; i < sound_buffers_.size(); ++i)
    {
        if (sound_buffers_[i] != NULL)
        {
            delete sound_buffers_[i];
        }
    }*/
}

void sound::set_bgm_volume(float volume)
{

    if (volume > 100)
    {
        volume = 100;
    }
    else if (volume < 0)
    {
        volume = 0;
    }
    volume_bgm_ = volume;
    background_music_->setVolume(volume);
    std::cout << "The music volume is: " << volume << std::endl;
}

void sound::set_sfx_volume(float volume)
{

    if (volume > 100)
    {
        volume = 100;
    }
    else if (volume < 0)
    {
        volume = 0;
    }
    volume_sfx_ = volume;
    sound_effects_->setVolume(volume);
    std::cout << "The sound volume is: " << volume <<std::endl;
}

float sound::get_bgm_volume()
{
    return background_music_->sf::Music::getVolume();
}

float sound::get_sfx_volume()
{
    return sound_effects_->sf::Sound::getVolume();
    //return volume_sfx_;
}

void sound::load_sfx(const std::string& file_path, const std::string& sfx_name)
{
    std::cout << "SOUND LOADED" << std::endl;
    sf::SoundBuffer *sound_buffer_ = new sf::SoundBuffer;
    sound_buffers_.insert (std::pair<std::string,
                            sf::SoundBuffer *>(sfx_name, sound_buffer_));
    sound_buffer_->loadFromFile(file_path);
    std::cout << file_path << std::endl;
}

void sound::load_bgm(const std::string& file_path, bool play_now, bool loop)
{
    std::cout << "MUSIC LOADED" << std::endl;
    background_music_ = new sf::Music;
    background_music_->openFromFile(file_path);
}

void sound::play_sfx(const std::string& sfx_name)
{
    sound_effects_ = new sf::Sound(*sound_buffers_.at(sfx_name));
    sound_effects_->setVolume(volume_sfx_);
    //sound_effects_->setLoop(true);
    sound_effects_->play();
    std::cout << "SFX_SOUND PLAYING" << std::endl;
    std::cout << " " << std::endl;
}

void sound::play_bgm()
{
    std::cout << "BGM_MUSIC PLAYING" << std::endl;
    std::cout << " " << std::endl;
    background_music_->setLoop(true);
    background_music_->play();
}

void sound::pause_bgm()
{
    std::cout << "BGM_MUSIC PAUSE" << std::endl;
    std::cout << " " << std::endl;
    background_music_->pause();
}

void sound::stop_bgm()
{
    background_music_->stop();
}

bool sound::get_bgm_playing()
{

}

void sound::unload_all()
{

    std::cout << "--SOUNDS UNLOADED--" << std::endl;
}
