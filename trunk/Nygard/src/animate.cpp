#include "animate.hpp"

animate::animate() : character_sprite_(NULL)
{
    //ctor
    sprite_height = 125;
    sprite_width = 125;

    sheet_width = 3000;
    sheet_height = 1000;

    sheet_columns = 0;
    sheet_row = 0;
}

animate::~animate()
{
    //dtor
}

void animate::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*character_sprite_, states);
}

