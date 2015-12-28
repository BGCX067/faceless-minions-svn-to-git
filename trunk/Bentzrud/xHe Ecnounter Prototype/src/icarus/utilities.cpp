#include <fstream>
#include "icarus/utilities.hpp"

namespace icarus
{
namespace utilities
{
bool file_exists(const std::string& path)
{
  std::ifstream ifile(path.c_str());
  return ifile;
}
bool is_number(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("0123456789.-") == std::string::npos);
}
}   // namespace utilities
}   // namespace icarus
