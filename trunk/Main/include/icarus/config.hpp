#ifndef ICARUS_CONFIG_HPP_
#define ICARUS_CONFIG_HPP_

#include <unordered_map>
#include <cstdlib>
#include "icarus/utils.hpp"

namespace icarus
{
class config
{
private:
    std::unordered_map<std::string, std::string> config_values_;
public:
    config() {}
    ~config() {}
    bool load_config_file(const std::string& path);
    template<typename T>
    T get_value(const std::string& key, const T& default_val, bool is_float = false) const
    {
        if (config_values_.count(key) == 0)
            return default_val;
        if (utils::is_number(config_values_.at(key)))
        {
            if (!is_float)
                return atoi(config_values_.at(key).c_str());
            return atof(config_values_.at(key).c_str());
        }
        else
        {
            return default_val;
        }
    }
    const std::string& get_value(const std::string& key, const std::string& default_val) const
    {
        if (config_values_.count(key) == 0)
            return default_val;
        return config_values_.at(key);
    }
};
}   // namespace icarus

#endif // ICARUS_CONFIG_HPP_
