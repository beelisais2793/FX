#pragma once

#include "tfp/config.hpp"
#include "tfp/math/Math.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>


namespace str {

/*!
 * @brief Splits a string into a list of strings using a delimiter.
 * @note Empty delimiters are skipped. For example, split("34::65:7", ':')
 * would yield the list ["34", "65", "7"] and **NOT** ["34", "", "65", "7"].
 * @param s The string to split
 * @param delim The delimiter character to use for the splitting.
 * @param elems A vector of strings to append the results to. Existing items
 * are not deleted.
 * @return Returns the same vector object passed in.
 */
std::vector<std::string>& split(const std::string& s,
                                char delim, std::vector<std::string>& elems);

/*!
 * @brief Splits a string into a list of strings using a delimiter.
 * @note Empty delimiters are skipped. For example, split("34::65:7", ':')
 * would yield the list ["34", "65", "7"] and **NOT** ["34", "", "65", "7"].
 * @param s The string to split
 * @param delim The delimiter character to use for the splitting.
 * @return Returns a vector of strings.
 */
std::vector<std::string> split(const std::string& s, char delim);

/*!
 * @brief Replaces all occurrences of search in subject with replace, and
 * returns a new copy.
 */
std::string replaceAll(std::string subject, const std::string& search, const std::string& replace);

/*!
 * @brief Converts any type that can be streamed to std::cout to a string.
 * @return Returns a std::string.
 */
template <class T>
std::string to_string(const T& x, unsigned numDigits=0)
{
    std::stringstream ss;
    if(numDigits > 0)
        ss << std::setprecision(numDigits);
    ss << std::dec << x;
    return ss.str();
}

/*!
 * @brief Removes whitespaces and carriage returns from the start and end of
 * the string.
 * @param str The string to strip.
 * @return Returns the stripped string.
 */
std::string strip(const std::string& str);

/*!
 * @brief Removes all occurrences of a character from a string.
 * @param str The string to process.
 * @param c The character to remove
 * @return A new, sanitized string.
 */
std::string remove(const std::string& str, char c);

/*!
 * @brief Removes all occurrences of a list of characters from a string.
 * @param str The string to process.
 * @param chars All characters that should be removed.
 * @return A new, sanitized string.
 */
std::string removeAll(const std::string& str, const std::string& chars);

/*!
 * @brief Returns true if the specified character is a digit.
 */
bool isDigit(char c);

/*!
 * @brief Returns true if the specified character is a whitespace.
 */
bool isWhiteSpace(char c);

/*!
 * @brief Returns true if the specified string is a number.
 * @note Includes scientific notations, such as 4.5e-6.
 */
bool isNumber(const std::string& str);

/*!
 * @brief Isolates all integers in a string and creates a list out of them.
 * @param str The string to process.
 * @return A list of strings where each string contains a number.
 */
std::vector<std::string> extractIntegers(const std::string& str);

/*!
 * @brief Compares to C strings to one another.
 *
 * This functions starts comparing the first character of each string. If they
 * are equal to one another, it continues with the following two pairs until
 * the characters differ or until a terminating null-character is reached.
 *
 * @param[in] str1 The first string.
 * @param[in] str2 The second string.
 * @return Returns the offset to the first character pair that does not match.
 * For example, comparing "abcde" to "ab2defgh" would yield 2. Comparing two
 * empty strings will yield 0.
 */
unsigned strcmpoff(const char* str1, const char* str2);

} // namespace str
