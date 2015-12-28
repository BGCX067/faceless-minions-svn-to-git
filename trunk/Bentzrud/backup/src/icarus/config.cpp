#include <iostream>

#include "icarus/config.hpp"
#include "icarus/utilities/file.hpp"

namespace icarus
{
bool config::load_config_file(const std::string& path)
{
    utilities::file cfg(path, false);

    std::string line = "";
    while(cfg.is_good())
    {
        line = cfg.get_line();
        if (line[0] == '[')
        {
            std::cout << line << std::endl;
            continue;
        }
        std::string key, value;
        key = line.substr(0, line.find('='));
        value = line.substr(line.find('=') + 1);
        std::pair<std::string, std::string> new_pair(key, value);
        config_values_.insert(new_pair);
        std::cout << "key: " << key << ", val: " << value << std::endl;
    }

    return cfg.close();
}
}   // namespace icarus
