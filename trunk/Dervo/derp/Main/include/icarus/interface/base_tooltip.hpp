#ifndef ICARUS_INTERFACE_BASE_TOOLTIP_HPP_
#define ICARUS_INTERFACE_BASE_TOOLTIP_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
namespace interface
{
namespace tooltip_type
{
enum value
{
    generic,
    encounter_skill,
    encounter_effect
};
}

class base_tooltip
{
protected:
    static std::vector<base_tooltip*> master_list_;
    tooltip_type::value tooltip_type_;
    utils::yth_node* tooltip_data_;
    sf::FloatRect hover_bounds_;
    bool enabled_;

public:
    base_tooltip();
    virtual ~base_tooltip();

    void set_tt_data(utils::yth_node* data) { tooltip_data_ = data; }
    void set_tt_enabled(bool enabled) { enabled_ = enabled; }
    bool is_tt_enabled() const { return enabled_; }
    const sf::FloatRect& get_tt_bounds() const { return hover_bounds_; }
    const utils::yth_node* get_tooltip_data() const { return tooltip_data_; }
    tooltip_type::value get_tooltip_type() const { return tooltip_type_; }
    static const std::vector<base_tooltip*>& get_list() { return master_list_; }
};
}   // namespace interface
}   // namespace icarus

#endif // ICARUS_INTERFACE_BASE_TOOLTIP_HPP_
