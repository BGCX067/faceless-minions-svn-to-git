#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>

#include "icarus/math/vector2.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/base_state.hpp"
//#include "icarus/base_state.hpp"
#include "icarus/menu/main_buttons.h"

namespace icarus
{
namespace menu
{
class main_menu :   public base_state
{
    public:
        main_menu(sf::RenderWindow *const window);
        virtual ~main_menu();
        //math::vector2i get_mouse_xy();
        virtual void pause_state();
        virtual void resume_state();
        void handle_input(const std::vector<sf::Event>& mouse_events);
        virtual void handle_logic();
        virtual void draw() const;
    protected:
    private:
         sf::RenderWindow *window_;
         main_buttons buttons_;
};
}   //namespace menu
}   //namespace icarus

#endif // MAIN_MENU_H
