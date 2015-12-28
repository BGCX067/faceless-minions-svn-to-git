#ifndef ICARUS_BASE_STATE_HPP_
#define ICARUS_BASE_STATE_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/utils/serializable.hpp"

namespace icarus
{
class base_state : public utils::serializable
{
protected:
    sf::RenderWindow *const window_;

public:
    base_state(sf::RenderWindow *const window) : window_(window) {}
    virtual ~base_state() {}

    virtual void pause_state() = 0;
    virtual void resume_state() = 0;

    virtual void save(std::ofstream& stream) = 0;
    virtual void load(std::ifstream& stream) = 0;

    virtual void handle_input(const std::vector<sf::Event>& key_events) = 0;
    virtual void handle_logic() = 0;
    virtual void draw() const = 0;
};
} // namespace icarus

#endif // ICARUS_BASE_STATE_HPP_
