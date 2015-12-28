#include "icarus/resource_handler.hpp"

namespace icarus
{
resource_handler::resource_handler()
{
}

resource_handler::~resource_handler()
{

}

bool resource_handler::load_texture(const std::string& file_path,
                                    const std::string& map_key)
{
    sf::Texture *texture_ptr = new sf::Texture();
    if (!texture_ptr->loadFromFile(file_path))
        return false;

    std::pair<std::string, sf::Texture*> new_pair(map_key, texture_ptr);
    texture_map_.insert(new_pair);
    return true;
}

sf::Texture* resource_handler::get_texture(const std::string& map_key) const
{
    return texture_map_.at(map_key);
}
}
