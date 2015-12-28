#include "icarus/sound.hpp"
#include "icarus/math.hpp"

namespace icarus
{
sound* sound::instance_ = NULL;
sound::sound()
:
    volume_sfx_(100.0f),
    volume_bgm_(100.0f),
    background_music_(NULL)
{
}

sound::~sound()
{
    if (background_music_ != NULL)
    {
        delete background_music_;
    }
    for (auto it = sound_buffers_.begin(); it != sound_buffers_.end(); ++it)
    {
        if (it->second != NULL)
            delete it->second;
    }
}

void sound::set_bgm_volume(float volume)
{
    volume_bgm_ = math::clamp(volume, 0.0f, 100.0f);
    if (background_music_ != NULL)
        background_music_->setVolume(volume_bgm_);
}

void sound::set_sfx_volume(float volume)
{
    volume_sfx_ = math::clamp(volume, 0.0f, 100.0f);
}

float sound::get_bgm_volume()
{
    return background_music_->sf::Music::getVolume();
}

float sound::get_sfx_volume()
{
    return volume_sfx_;
}

void sound::load_sfx(const std::string& file_path, const std::string& sfx_name)
{
    sf::SoundBuffer* sound_buffer_ = new sf::SoundBuffer();
    sound_buffer_->loadFromFile(file_path);
    sound_buffers_.insert(std::pair<std::string,
                          sf::SoundBuffer*>(sfx_name, sound_buffer_));
}

void sound::load_bgm(const std::string& file_path, bool play_now, bool loop)
{
    if (background_music_ == NULL)
        background_music_ = new sf::Music();
    background_music_->openFromFile(file_path);
    if (play_now)
        play_bgm(loop);
}

void sound::play_sfx(const std::string& sfx_name)
{
    sound_effects_.push(sf::Sound(*sound_buffers_.at(sfx_name)));
    sound_effects_.back().setVolume(volume_sfx_);
    sound_effects_.back().play();
}

void sound::play_bgm(bool loop)
{
    if (background_music_ == NULL)
        return;
    background_music_->setLoop(loop);
    background_music_->setVolume(volume_bgm_);
    background_music_->play();
}

void sound::pause_bgm()
{
    if (background_music_ == NULL)
        return;
    background_music_->pause();
}

void sound::stop_bgm()
{
    if (background_music_ == NULL)
        return;
    background_music_->stop();
}

bool sound::get_bgm_playing()
{
    if (background_music_ == NULL)
        return false;
    return (background_music_->getStatus() == sf::SoundSource::Playing);
}

void sound::unload_all()
{
    if (background_music_ != NULL)
    {
        delete background_music_;
        background_music_ = NULL;
    }
    for (auto it = sound_buffers_.begin(); it != sound_buffers_.end(); ++it)
    {
        if (it->second != NULL)
            delete it->second;
    }
    sound_buffers_.clear();
    while (!sound_effects_.empty())
        sound_effects_.pop();
}

void sound::update()
{
    if (!sound_effects_.empty() && sound_effects_.front().getStatus() != sf::Sound::Playing)
        sound_effects_.pop();
}
} // namespace icarus
