#ifndef ANIMATE_H
#define ANIMATE_H
#include <SFML/Graphics.hpp>
#include <iostream>

class animate: public sf::Drawable
{
    private:
    protected:
        unsigned int sprite_width;
        unsigned int sprite_height;
        unsigned int sheet_width;
        unsigned int sheet_height;
        unsigned int sheet_columns;
        unsigned int sheet_row;
    public:
        animate();
        virtual ~animate();

    sf::Sprite *character_sprite_;
    sf::Vector2f character_position_;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif // ANIMATE_H
