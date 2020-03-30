//
// Created by keszocze on 27.09.18.
//

#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace abo::util {
/**
 * @brief Creates an array of cstrings from a vector of strings.
 * @param vec Vector of strings to be put in an array
 * @return Pointer to the array of strings
 * @note The callee is responsible for deleting the array (possible using {@see del_arr})
 *
 * This method is only necessary as CUDD basically is a plain c library.
 */
char** vec_to_arr(const std::vector<std::string>& vec);

/**
 * @brief Deletes an array of cstrings
 * @param arr The array to delete
 * @param n The number of entries in the array
 *
 * This is just a convenience method.
 */
void del_arr(char** arr, std::size_t n);

template <class Container>
/**
 * @brief Splits a string on whitespaces
 * @tparam Container The container type in which the tokens are placed
 * @param str The string to split
 * @param cont The container in which the tokens are placed
 */
void split(const std::string& str, Container& cont)
{
    std::istringstream iss(str); // aus sstream?
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(),
              std::back_inserter(cont));
}

} // namespace abo::util
