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

bool resource_handler::add_root_node(utils::yth_node* root, const std::string& key)
{
    if (root_node_map_.count(key) > 0)
        return false;

    std::pair<std::string, utils::yth_node*> new_pair(key, root);
    root_node_map_.insert(new_pair);

    return !(root_node_map_.count(key) <= 0);
}

utils::yth_node* resource_handler::get_root_node(const std::string& key) const
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
    // TODO:: only enable smooting if image needs scaling!
    texture_ptr->setSmooth(true);
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

bool resource_handler::load_font(const std::string& name,
                                 const std::string& file_path)
{
    sf::Font* font_ptr = new sf::Font();
    if (!font_ptr->loadFromFile(file_path))
        return false;
    std::pair<std::string, sf::Font*> new_pair(name, font_ptr);
    font_map_.insert(new_pair);
    return true;
}

sf::Font* resource_handler::get_font(const std::string& name) const
{
    if (font_map_.count(name))
        return font_map_.at(name);
    return NULL;
}

bool resource_handler::load_shader(const std::string& name,
                                   const std::string& file_path)
{
    sf::Shader* shader_ptr = new sf::Shader();
    if (file_path.find(".frag") != std::string::npos)
    {
        if (!shader_ptr->loadFromFile(file_path, sf::Shader::Fragment))
            return false;
    }
    else if (file_path.find(".vert") != std::string::npos)
    {
        if (!shader_ptr->loadFromFile(file_path, sf::Shader::Vertex))
            return false;
    }
    else
    {
        return false;
    }
    std::pair<std::string, sf::Shader*> new_pair(name, shader_ptr);
    shader_map_.insert(new_pair);
    return true;
}

sf::Shader* resource_handler::get_shader(const std::string& name) const
{
    if (shader_map_.count(name))
        return shader_map_.at(name);
    return NULL;
}
}
