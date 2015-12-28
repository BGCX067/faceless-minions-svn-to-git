#ifndef ICARUS_utils_FILE_HPP_
#define ICARUS_utils_FILE_HPP_

#include <iostream>
#include <fstream>
#include <string>

namespace icarus
{
namespace utils
{
class file
{
private:
    std::fstream file_stream_;
public:
    file(const std::string& path, bool binary = true);
    file();
    ~file();

    bool open(const std::string& path, bool binary = true);
    bool close();
    std::string get_line();

    bool is_good() const { return file_stream_.good(); }
    template<typename T>
    void read_into(T& output, size_t bytes = 0)
    {
        size_t read_bytes = (bytes == 0 ? sizeof(output) : bytes);
        file_stream_.read(output, read_bytes);
    }
};
}   // namespace utils
}   // namespace icarus

#endif // ICARUS_utils_FILE_HPP_
