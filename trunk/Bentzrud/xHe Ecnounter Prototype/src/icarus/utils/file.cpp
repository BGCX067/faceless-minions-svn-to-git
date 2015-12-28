#include "icarus/utils/file.hpp"

namespace icarus
{
namespace utils
{
file::file(const std::string& path, bool binary)
{
    open(path, binary);
}
file::file()
{

}
file::~file()
{
    file_stream_.close();
}

bool file::open(const std::string& path, bool binary)
{
    if (binary)
        file_stream_.open(path.c_str(), std::ios::in |
                                        std::ios::out |
                                        std::ios::binary);
    else
        file_stream_.open(path.c_str());
    return file_stream_.good();
}
bool file::close()
{
    if (file_stream_.is_open())
    {
        file_stream_.close();
        return true;
    }
    return false;
}
std::string file::get_line()
{
    std::string output;
    std::getline(file_stream_, output);
    return output;
}
}   // namespace utils
}   // namespace icarus
