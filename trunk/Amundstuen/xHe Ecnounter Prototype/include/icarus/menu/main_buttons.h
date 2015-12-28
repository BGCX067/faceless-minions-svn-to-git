
#ifndef MAIN_BUTTONS_H
#define MAIN_BUTTONS_H

#include <SFML/Graphics.hpp>


namespace icarus
{
namespace menu
{
class main_buttons :    public sf::Drawable
{
    public:
        main_buttons();
        virtual ~main_buttons();
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            target.draw(back_image, states);
            target.draw(spr_title, states);
            target.draw(spr_button_new, states);
            target.draw(spr_button_load, states);
            target.draw(spr_button_options, states);
            target.draw(spr_button_exit, states);

        }
        void handle_input(const std::vector<sf::Event>& mouse_events);
        void set_window(sf::RenderWindow* window);

    protected:
    private:

        sf::RenderWindow* window_;
        //Textures
        sf::Texture txt_back_image;
        sf::Texture txt_button_new;
        sf::Texture txt_button_load;
        sf::Texture txt_button_options;
        sf::Texture txt_button_exit;
        sf::Texture title_test;

        //Sprites
        sf::Sprite back_image;
        sf::Sprite spr_title;
        sf::Sprite spr_button_new;
        sf::Sprite spr_button_load;
        sf::Sprite spr_button_options;
        sf::Sprite spr_button_exit;
};
}   //namespace menu
}   //namespace icarus
#endif // MAIN_BUTTONS_H
