#include "icarus/menu/main_buttons.h"
#include "icarus/menu/main_menu.h"
#include "icarus/encounter/state.hpp"
//#include "icarus/menu/main.h"

namespace icarus
{
namespace menu
{
void main_buttons::set_window(sf::RenderWindow* window)
{
    window_ = window;
    std::cout << window << std::endl;
}
main_buttons::main_buttons()
{

    txt_back_image.loadFromFile("images/backgrounds/menu_background.jpg");
    txt_back_image.setSmooth(true);


    txt_button_new.loadFromFile("Sprites/txt_button_new.png");
    txt_button_new.setSmooth(true);


    txt_button_load.loadFromFile("Sprites/txt_button_load.png");
    txt_button_load.setSmooth(true);

    txt_button_options.loadFromFile("Sprites/txt_button_options.png");
    txt_button_options.setSmooth(true);


    txt_button_exit.loadFromFile("Sprites/Button_exit.png");
    txt_button_exit.setSmooth(true);


    title_test.loadFromFile("Sprites/title_test.png");
    title_test.setSmooth(true);

    //BACKGROUND
    back_image.setTexture(txt_back_image);
    back_image.setTextureRect(sf::IntRect(0, 0, 1300, 1000));
    back_image.setColor(sf::Color(255, 255, 255, 255));
    //back_image.setPosition(200, 200);

    //TITLE
    spr_title.setTexture(title_test);
    spr_title.setTextureRect(sf::IntRect(0, 0, 500, 500));
    spr_title.setColor(sf::Color(255, 255, 255, 255));
    spr_title.setOrigin(101.5, 111.5);
    spr_title.setPosition(300, 100);


    //NEW_BUTTON
    spr_button_new.setTexture(txt_button_new);
    spr_button_new.setTextureRect(sf::IntRect(0, 0, 203, 175));
    spr_button_new.setColor(sf::Color(255, 200, 140, 255));
    spr_button_new.setOrigin(101.5, 111.5);
    spr_button_new.setPosition(1150, 130);

    //LOAD_BUTTON
    spr_button_load.setTexture(txt_button_load);
    spr_button_load.setTextureRect(sf::IntRect(0, 0, 203, 175));
    spr_button_load.setColor(sf::Color(255, 200, 140, 255));
    spr_button_load.setOrigin(101.5, 111.5);
    spr_button_load.setPosition(1150, 310);

    //OPTIONS_BUTTON
    spr_button_options.setTexture(txt_button_options);
    spr_button_options.setTextureRect(sf::IntRect(0, 0, 203, 175));
    spr_button_options.setColor(sf::Color(255, 200, 140, 255));
    spr_button_options.setOrigin(101.5, 111.5);
    spr_button_options.setPosition(1150, 490);

    //EXIT_BUTTON
    spr_button_exit.setTexture(txt_button_exit);
    spr_button_exit.setTextureRect(sf::IntRect(0, 0, 227, 133));
    spr_button_exit.setColor(sf::Color(255, 200, 140, 255));
    spr_button_exit.setOrigin(113.5, 66.5);
    spr_button_exit.setPosition(1150, 630);
    //std::cout << "here01" << std::endl;
}

main_buttons::~main_buttons()
{
    //dtor
}

void main_buttons::handle_input(const std::vector<sf::Event>& mouse_events)
{
    for(unsigned i =0; i < mouse_events.size(); i++)
    {
        //sf::RenderWindow *window_;

        float mouse_x_ = mouse_events[i].mouseButton.x;
        float mouse_y_ = mouse_events[i].mouseButton.y;
        sf::Vector2i position = sf::Mouse::getPosition();

        if (mouse_events[i].type == sf::Event::MouseButtonPressed)
        {
            if (mouse_events[i].mouseButton.button == sf::Mouse::Left)
            {
                if (spr_button_new.getGlobalBounds()
                                            .contains(mouse_x_, mouse_y_))
                {
                    spr_button_new.rotate(2);
                    std::cout << "maN "<< std::endl;
                    state_manager::get_instance().change_state(new encounter::state(window_));
                    std::cout << "here" << std::endl;
                }
                if (spr_button_load.getGlobalBounds()
                                            .contains(mouse_x_, mouse_y_))
                {
                    spr_button_load.rotate(-12);
                    std::cout << "Load "<< std::endl;
                }
                if (spr_button_options.getGlobalBounds()
                                            .contains(mouse_x_, mouse_y_))
                {
                    spr_button_options.rotate(23);
                    std::cout << "OptiOns "<< std::endl;

                }
                if (spr_button_exit.getGlobalBounds()
                                            .contains(mouse_x_, mouse_y_))
                {
                    std::cout << "ExIT "<< std::endl;
                    return;
                }
            }
        }

    }
//std::cout << "here02" << std::endl;
}

} //namespace menu
} //namespace icarus
