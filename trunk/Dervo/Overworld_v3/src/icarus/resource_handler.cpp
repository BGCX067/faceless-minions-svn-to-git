#include "icarus/resource_handler.hpp"

namespace icarus
{
resource_handler* resource_handler::instance_ = NULL;
resource_handler::resource_handler()
{
}

resource_handler::~resource_handler()
{

}

bool resource_handler::add_root_node(utilities::yth_node* root, const std::string& key)
{
    if (root_node_map_.count(key) > 0)
        return false;

    std::pair<std::string, utilities::yth_node*> new_pair(key, root);
    root_node_map_.insert(new_pair);

    return !(root_node_map_.count(key) <= 0);
}

utilities::yth_node* resource_handler::get_root_node(const std::string& key) const
{
    if (root_node_map_.count(key) <= 0)
        return NULL;
    return root_node_map_.at(key);
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
        if (!resource_handler::get()->load_texture(file_path))
            return NULL;
    return texture_map_.at(file_path);
}
}
