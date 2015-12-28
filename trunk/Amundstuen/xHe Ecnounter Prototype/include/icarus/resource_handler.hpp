
#ifndef ICARUS_RESOURCE_HANDLER_HPP_
#define ICARUS_RESOURCE_HANDLER_HPP_

#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace icarus
{
class resource_handler
{
private:
    std::unordered_map<std::string, sf::Texture*> texture_map_;

    resource_handler();
    resource_handler(resource_handler const&);
    void operator=(resource_handler const&);
public:
    ~resource_handler();
    static resource_handler& get_instance()
    {
        static resource_handler instance;
        return instance;
    }

    bool load_texture(const std::string& file_path);
    sf::Texture* get_texture(const std::string& file_path) const;
};
} // namespace icarus

#endif // ICARUS_RESOURCE_HANDLER_HPP_
