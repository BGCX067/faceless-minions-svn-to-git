
#ifndef ICARUS_RESOURCE_HANDLER_HPP_
#define ICARUS_RESOURCE_HANDLER_HPP_

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
class resource_handler
{
private:
    std::unordered_map<std::string, sf::Shader*> shader_map_;
    std::unordered_map<std::string, sf::Font*> font_map_;
    std::unordered_map<std::string, sf::Texture*> texture_map_;
    std::unordered_map<std::string, utils::yth_node*> root_node_map_;
    static resource_handler* instance_;

    resource_handler();
    resource_handler(resource_handler const&);
    void operator=(resource_handler const&);
public:
    virtual ~resource_handler();
    static resource_handler* get()
    {
        if (!instance_)
        instance_ = new resource_handler();
        return instance_;
    }

    bool add_root_node(utils::yth_node* root, const std::string& key);
    utils::yth_node* get_root_node(const std::string& key) const;

    bool load_texture(const std::string& file_path);
    sf::Texture* get_texture(const std::string& file_path) const;

    bool load_font(const std::string& name, const std::string& file_path);
    sf::Font* get_font(const std::string& name) const;

    bool load_shader(const std::string& name, const std::string& file_path);
    sf::Shader* get_shader(const std::string& name) const;
};
} // namespace icarus

#endif // ICARUS_RESOURCE_HANDLER_HPP_
