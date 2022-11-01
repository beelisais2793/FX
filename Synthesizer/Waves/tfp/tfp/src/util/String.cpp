#include "tfp/util/String.hpp"

#include <algorithm>


namespace str {

// ----------------------------------------------------------------------------
std::vector<std::string>& split(const std::string &s, char delim,
                                std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if(item.length() > 0)
            elems.push_back(item);
    }
    return elems;
}

// ----------------------------------------------------------------------------
std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

// ----------------------------------------------------------------------------
std::string replaceAll(std::string subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

// ----------------------------------------------------------------------------
std::string strip(const std::string& str)
{
    static const char* whiteSpace = "\r\t\n ";
    std::string cp = str;

    // Clean up from start
    std::size_t pos = cp.find_first_not_of(whiteSpace);
    if(pos != 0)
        cp.erase(0, pos);

    // Clean up from end
    pos = cp.find_last_not_of(whiteSpace);
    if(pos != std::string::npos)
        cp.erase(pos + 1);
    else
        cp.clear(); // string is all whitespace

    return cp;
}

// ----------------------------------------------------------------------------
std::string remove(const std::string& str, char c)
{
    std::string cp = str;
    cp.erase(std::remove(cp.begin(), cp.end(), c), cp.end());
    return cp;
}

// ----------------------------------------------------------------------------
std::string removeAll(const std::string& str, const std::string& chars)
{
    std::string cp = str;
    for(std::string::const_iterator it = chars.begin(); it != chars.end(); ++it)
        cp = remove(cp, *it);
    return cp;
}

// ----------------------------------------------------------------------------
bool isDigit(char c)
{
    return (c > 47 && c < 58);
}

// ----------------------------------------------------------------------------
bool isWhiteSpace(char c)
{
    return (c == ' ' || c == '\t');
}

// ----------------------------------------------------------------------------
bool isNumber(const std::string& str)
{
    static const std::string validSymbols = "-+.e \t";
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        if(!isDigit(*it) && validSymbols.find(*it) == std::string::npos)
            return false;

    return true;
}

// ----------------------------------------------------------------------------
std::vector<std::string> extractIntegers(const std::string& str)
{
    std::vector<std::string> ret;
    std::string number;
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if(isDigit(*it))
            number += *it;
        else
            if(number.length() > 0)
            {
                ret.push_back(number);
                number.clear();
            }
    }

    return ret;
}

// ----------------------------------------------------------------------------
unsigned strcmpoff(const char* str1, const char* str2)
{
    const char* store = str1;
    while(*str1 != '\0' && *str1 == *str2)
    {
        str1++;
        str2++;
    }

    return str1 - store;
}

} // namespace str
