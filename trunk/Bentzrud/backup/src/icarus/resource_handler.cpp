#include "icarus/resource_handler.hpp"

namespace icarus
{
resource_handler::resource_handler()
{
}

resource_handler::~resource_handler()
{

}

bool resource_handler::load_texture(const std::string& file_path)
{
    sf::Texture *texture_ptr = new sf::Texture();
    if (!texture_ptr->loadFromFile(file_path))
        return false;

    std::pair<std::string, sf::Texture*> new_pair(file_path, texture_ptr);
    texture_map_.insert(new_pair);
    return true;
}

sf::Texture* resource_handler::get_texture(const std::string& file_path) const
{
    if (texture_map_.count(file_path) <= 0)
        if (!resource_handler::get_instance().load_texture(file_path))
            return NULL;
    return texture_map_.at(file_path);
}
}
