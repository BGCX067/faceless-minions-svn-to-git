#include "character_base.hpp"

character_base::character_base()
{
    std::cout << "This shit is called, yo" << std::endl;
}

character_base::character_base(unsigned sprite_height,
                            unsigned sprite_width,
                            unsigned sheet_height,
                            unsigned sheet_width)
{
    //Animation Attribs
    sprite_height = 125;
    sprite_width = 125;

    sheet_width = 3000;
    sheet_height = 1000;

    sheet_column = 0;
    sheet_row = 0;
}

character_base::~character_base()
{

}
/*
void character_base::draw(sf::RenderTarget& target,
                          sf::RenderStates states) const
{
    target.draw(*character_sprite_, states);
    std::cout << "lolleru nao?" << std::endl;
}

void character_base::animate()
{
    character_sprite_->setOrigin((sprite_height*0.5), (sprite_width*0.5));
    character_sprite_->setTextureRect
    (sf::IntRect(sheet_column, sheet_row, sprite_width, sprite_height));

    sheet_column += sprite_width;
    if(sheet_column >= sheet_width)
    {
        sheet_column = 0;
    }
    if(sheet_row >= sheet_height)
    {
        sheet_row = 0;
    }
}
*/
