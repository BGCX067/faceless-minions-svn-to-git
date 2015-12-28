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
}   // namespace utilities
}   // namespace icarus
