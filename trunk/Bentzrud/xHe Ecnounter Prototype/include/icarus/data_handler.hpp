#ifndef ICARUS_DATA_HANDLER_HPP_
#define ICARUS_DATA_HANDLER_HPP_

#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace icarus
{
class data_handler
{
private:
    std::unordered_map<std::string, sf::Texture*> texture_map_;

    data_handler();
    data_handler(data_handler const&);
    void operator=(data_handler const&);
public:
    ~data_handler();
    static data_handler& get_instance()
    {
        static data_handler instance;
        return instance;
    }


    bool load_texture(const std::string& file_path);
    sf::Texture* get_texture(const std::string& file_path) const;
};
} // namespace icarus

#endif // ICARUS_DATA_HANDLER_HPP_
