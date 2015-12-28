#ifndef ICARUS_MENU_MENU_STATE_HPP_
#define ICARUS_MENU_MENU_STATE_HPP_

#include "icarus/menu/button.hpp"
#include "icarus/base_state.hpp"
#include "icarus/ui/screen_fade.hpp"

namespace icarus
{
namespace menu
{
class menu_state : public base_state
{
private:
    std::vector<button*> buttons_;
    sf::Sprite* background_;
    ui::screen_fade fader_;
    int clicked_index_;
public:
    menu_state(sf::RenderWindow *const window);
    virtual ~menu_state();
    virtual void pause_state();
    virtual void resume_state();

    virtual void save(std::ofstream& stream) {};
    virtual void load(std::ifstream& stream) {};

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
}   // namespace menu
}   // namespace icarus

#endif // ICARUS_MENU_MENU_STATE_HPP_
