#ifndef ICARUS_AI_TIMER_NODE_HPP_
#define ICARUS_AI_TIMER_NODE_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class timer_node : public behavior_node
{
private:
    sf::Clock timer_;
    float seconds_;
public:
    timer_node(float seconds) : behavior_node(), seconds_(seconds) {}
    virtual ~timer_node();
    virtual status::value execute();
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_TIMER_NODE_HPP_

