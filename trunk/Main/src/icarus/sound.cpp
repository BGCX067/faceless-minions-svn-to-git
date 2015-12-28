#include "icarus/sound.hpp"
#include "icarus/math.hpp"
#include "icarus/utils.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
sound* sound::instance_ = NULL;
sound::sound()
:
    VOLUME_BASE_(175.0f),
    sound_enabled_(true),
    bgm_enabled_(true),
    sfx_enabled_(true),
    volume_master_(1.0f),
    volume_sfx_(1.0f),
    volume_bgm_(1.0f),
    background_music_(NULL)
{
    set_master_volume(1.0f);
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

void sound::set_master_volume(float volume)
{
    volume_master_ = math::clamp(volume, 0.0f, 1.0f);
    sf::Listener::setGlobalVolume(((pow(VOLUME_BASE_, volume_master_)-1.0f)/(VOLUME_BASE_-1.0f)) * 100.0f);
}

void sound::set_bgm_volume(float volume)
{
    volume_bgm_ = math::clamp(volume, 0.0f, 1.0f);
    if (background_music_ != NULL)
        background_music_->setVolume(((pow(VOLUME_BASE_, volume_bgm_)-1.0f)/(VOLUME_BASE_-1.0f)) * 100.0f);
}

void sound::set_sfx_volume(float volume)
{
    volume_sfx_ = math::clamp(volume, 0.0f, 1.0f);
}

float sound::get_bgm_volume()
{
    return volume_bgm_;
}

float sound::get_sfx_volume()
{
    return volume_sfx_;
}

float sound::get_master_volume()
{
    return volume_master_;
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
    if (sound_buffers_.count(sfx_name) <= 0 || !sound_enabled_ || !sfx_enabled_)
        return;
    sound_effects_.push(sf::Sound(*sound_buffers_.at(sfx_name)));
    sound_effects_.back().setVolume(((pow(VOLUME_BASE_, volume_sfx_)-1.0f)/(VOLUME_BASE_-1.0f)) * 100.0f);
    sound_effects_.back().play();
}
void sound::play_random_sfx(const std::string& sfx_name)
{
    if (!sound_enabled_ || !sfx_enabled_)
        return;
    utils::yth_node* sfx = resource_handler::get()->get_root_node("audio")->child("sfx");
    if (sfx->child_count(sfx_name) == 1 && sound_buffers_.count(sfx_name) > 0)
    {
        sound_effects_.push(sf::Sound(*sound_buffers_.at(sfx_name)));
        sound_effects_.back().setVolume(((pow(VOLUME_BASE_, volume_sfx_)-1.0f)/(VOLUME_BASE_-1.0f)) * 100.0f);
        sound_effects_.back().play();
    }
    else if (sfx->child_count(sfx_name) > 1)
    {
        sound_effects_.push(sf::Sound(*sound_buffers_.at(sfx_name + utils::to_str(rand()%sfx->child_count(sfx_name)))));
        sound_effects_.back().setVolume(((pow(VOLUME_BASE_, volume_sfx_)-1.0f)/(VOLUME_BASE_-1.0f)) * 100.0f);
        sound_effects_.back().play();
    }
}

void sound::play_bgm(bool loop)
{
    if (background_music_ == NULL || !sound_enabled_ || !bgm_enabled_)
        return;
    background_music_->setLoop(loop);
    background_music_->setVolume(((pow(VOLUME_BASE_, volume_bgm_)-1.0f)/(VOLUME_BASE_-1.0f)) * 100.0f);
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
