#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H
#include <string>
#include <SFML/Graphics.hpp>


class image_handler
{
private:


    image_handler();
    image_handler(image_handler const&);
    void operator=(image_handler const&);

protected:

public:
    ~image_handler();
    static image_handler& get_instance()
    {
        static image_handler instance;
        return instance;
    }

    bool load_image(const std::string& file_path, const std::string& map_key);
    sf::Texture* get_image(const std::string& map_key) const;
};

#endif // IMAGE_HANDLER_H
