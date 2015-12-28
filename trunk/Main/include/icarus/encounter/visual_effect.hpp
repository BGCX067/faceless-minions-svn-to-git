#ifndef ICARUS_ENCOUNTER_VISUAL_EFFECT_HPP_
#define ICARUS_ENCOUNTER_VISUAL_EFFECT_HPP_

#include "icarus/encounter/character_base.hpp"
#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
namespace encounter
{
class visual_effect : public animate
{
private:
    bool loop_;
    float timer_;
    float time_remaining_;
    bool active_;
    unsigned loop_counter_;
    bool loop_counted_;

public:
    visual_effect(math::vector2f position,
                  utils::yth_node* data,
                  bool loop,
                  float timer = 0.0f,
                  bool unscaled = true);
    virtual ~visual_effect();
    void update(double delta);
    void reset();
    void stop();
    bool is_active() const { return active_; }
};
}   // namespace encounter
} // namespace icarus
#endif // ICARUS_ENCOUNTER_VISUAL_EFFECT_HPP_
