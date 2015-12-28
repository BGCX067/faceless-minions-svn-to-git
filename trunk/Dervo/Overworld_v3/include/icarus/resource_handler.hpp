
#ifndef ICARUS_RESOURCE_HANDLER_HPP_
#define ICARUS_RESOURCE_HANDLER_HPP_

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "icarus/utilities/yth_handler.hpp"

namespace icarus
{
class resource_handler
{
private:
    std::unordered_map<std::string, sf::Texture*> texture_map_;
    std::unordered_map<std::string, utilities::yth_node*> root_node_map_;
    static resource_handler* instance_;

    resource_handler();
    resource_handler(resource_handler const&);
    void operator=(resource_handler const&);
public:
    ~resource_handler();
    static resource_handler* get()
    {
        if (!instance_)
            instance_ = new resource_handler();
        return instance_;
    }

    bool add_root_node(utilities::yth_node* root, const std::string& key);
    utilities::yth_node* get_root_node(const std::string& key) const;

    bool load_texture(const std::string& file_path);
    sf::Texture* get_texture(const std::string& file_path) const;
};
} // namespace icarus

#endif // ICARUS_RESOURCE_HANDLER_HPP_
