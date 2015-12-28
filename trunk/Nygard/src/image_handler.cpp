#include "image_handler.hpp"

image_handler::image_handler()
{
}

image_handler::~image_handler()
{
}

bool image_handler::load_image(const std::string& file_path,
                               const std::string& map_key)
{
    sf::Texture *texture_ptr = new sf::Texture();
    if (!texture_ptr->loadFromFile(file_path))
    {
        return false;
    }

    std::pair<std::string, sf::Texture*> new_pair(map_key, texture_ptr);
}
