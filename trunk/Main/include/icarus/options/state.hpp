#ifndef ICARUS_OPTIONS_STATE_HPP_
#define ICARUS_OPTIONS_STATE_HPP_

#include "icarus/base_state.hpp"
#include "icarus/ui/slider.hpp"
#include "icarus/ui/checkbox.hpp"

namespace icarus
{
namespace options
{
class state : public icarus::base_state
{
private:
    sf::Text options_title_;
    sf::Text return_to_menu_;
    sf::Text quit_game_;

    sf::Text master_title_;
    sf::Text master_volume_label_;
    ui::slider master_volume_;
    sf::Text master_enabled_label_;
    ui::checkbox master_enabled_;

    sf::Text bgm_title_;
    sf::Text bgm_volume_label_;
    ui::slider bgm_volume_;
    sf::Text bgm_enabled_label_;
    ui::checkbox bgm_enabled_;

    sf::Text sfx_title_;
    sf::Text sfx_volume_label_;
    ui::slider sfx_volume_;
    sf::Text sfx_enabled_label_;
    ui::checkbox sfx_enabled_;

    sf::Text video_options_;
    sf::Text fullscreen_label_;
    sf::Text hd_label_;
    sf::Text vsync_label_;
    ui::checkbox fullscreen_;
    ui::checkbox hd_;
    ui::checkbox vsync_;

    void save_changes();

public:
    state(sf::RenderWindow *const window);
    virtual ~state();

    virtual void pause_state();
    virtual void resume_state();

    virtual void save(std::ofstream& stream) {}
    virtual void load(std::ifstream& stream) {}

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
}   // namespace options
}   // namespace icarus

#endif // ICARUS_OPTIONS_STATE_HPP_

