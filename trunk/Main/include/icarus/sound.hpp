#ifndef ICARUS_SOUND_HPP_
#define ICARUS_SOUND_HPP_
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <queue>

namespace icarus
{
class sound
{
private:
    const float VOLUME_BASE_;
    bool sound_enabled_;
    bool bgm_enabled_;
    bool sfx_enabled_;
    float volume_master_;
    float volume_sfx_;
    float volume_bgm_;
    sf::Music* background_music_;
    std::queue<sf::Sound> sound_effects_;
    std::unordered_map<std::string, sf::SoundBuffer*> sound_buffers_;
    static sound* instance_;

    sound();
    sound(sound const&);
    void operator=(sound const&);
public:
    virtual ~sound();
    static sound* get()
    {
        if (!instance_)
            instance_ = new sound();
        return instance_;
    }
    void set_bgm_volume(float volume);
    void set_sfx_volume(float volume);
    void set_master_volume(float volume);
    float get_bgm_volume();
    float get_sfx_volume();
    float get_master_volume();
    void load_sfx(const std::string& file_path, const std::string& sfx_name);
    void load_bgm(const std::string& file_path, bool play_now, bool loop);
    void play_sfx(const std::string& sfx_name);
    void play_random_sfx(const std::string& sfx_name);
    void play_bgm(bool loop);
    void pause_bgm();
    void stop_bgm();
    bool get_bgm_playing();
    void unload_all();
    void update();

    void set_sound_enabled(bool status) { sound_enabled_ = status; }
    void set_bgm_enabled(bool status) { bgm_enabled_ = status; }
    void set_sfx_enabled(bool status) { sfx_enabled_ = status; }
    bool get_sound_enabled() const { return sound_enabled_; }
    bool get_bgm_enabled() const { return bgm_enabled_; }
    bool get_sfx_enabled() const { return sfx_enabled_; }
};
} // namespace icarus
#endif // ICARUS_SOUND_HPP_
