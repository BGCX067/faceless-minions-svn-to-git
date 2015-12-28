#ifndef CHARACTER_HERO_H
#define CHARACTER_HERO_H
#include "character_base.hpp"

class character_hero : public character_base
{
public:
    character_hero();
    character_hero(unsigned sprite_height,
                   unsigned sprite_width,
                   unsigned sheet_height,
                   unsigned sheet_width);
    virtual ~character_hero();

protected:

private:
};

#endif // CHARACTER_HERO_H
