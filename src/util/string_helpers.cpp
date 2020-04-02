#include "string_helpers.hpp"

#include <cstring>

namespace abo::util {

char** vec_to_arr(const std::vector<std::string>& vec)
{

    char** arr = new char*[vec.size()];

    for (std::size_t i = 0; i < vec.size(); i++)
    {
        std::size_t n = vec[i].size() + 1;
        arr[i] = new char[n];
        strncpy(arr[i], vec[i].c_str(), n);
    }

    return arr;
}

void del_arr(char** arr, std::size_t n)
{
    for (std::size_t i = 0; i < n; i++)
    {
        delete[] arr[i];
    }
    delete[] arr;
}

} // namespace abo::util
