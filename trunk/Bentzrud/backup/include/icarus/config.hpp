#ifndef ICARUS_CONFIG_HPP_
#define ICARUS_CONFIG_HPP_

#include <unordered_map>

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
    const T& get_config_value(const std::string& key) const
    {
        if (config_values_.count(key) > 0)
            return static_cast<T>(config_values_.at(key));
        return static_cast<T>(NULL);
    }
};
}   // namespace icarus

#endif // ICARUS_CONFIG_HPP_
