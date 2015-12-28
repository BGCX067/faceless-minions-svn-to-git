
#include "icarus/menu/main_menu.h"

namespace icarus
{
namespace menu
{
main_menu::main_menu(sf::RenderWindow *const window)  :
    base_state(window)
{
    window_ = window;
    buttons_.set_window(window);
    //std::cout << "here03" << std::endl;
}
main_menu::~main_menu()
{

}
void main_menu::pause_state()
{
}
void main_menu::resume_state()
{
}
void main_menu::handle_logic()
{
}
void main_menu::handle_input(const std::vector<sf::Event>& mouse_events)
{
    buttons_.handle_input(mouse_events);
}
void main_menu::draw() const
{
    //std::cout << "here04" << std::endl;
    window_->draw(buttons_);
    //std::cout << "here05" << std::endl;
}

}   //namespace menu
}   //namespace icarus
