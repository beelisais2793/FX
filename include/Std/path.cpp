#include "StdPosix.h"

// C
#include <limits.h>
#include <stdlib.h>
#include <string.h>

using namespace Std;

std::string
basename(const std::string& _path)
{
    const char* path = _path.c_str();
    std::string base;

    // An empty path is just curdir
    if (path[0] == '\0')
    {
        return ".";
    }

    size_t len = strlen(path);
    const char* start;
    const char* end = path + len - 1;

    // Remove trailing slashes
    while (end > path && *end == '/')
    {
        --end;
    }

    // If we had all slashes
    if (end == path && path[0] == '/')
    {
        base = "/";
    }

    // Work backwords to find the start
    start = end;

    while (start > path && *(start - 1) != '/')
    {
        --start;
    }

    base.assign(start, end - start + 1);
    return base;
}

std::string
 dirname(const std::string& _path)
{
    const char* path = _path.c_str();
    std::string dir;

    // An empty path is just curdir
    if (path[0] == '\0')
    {
        return ".";
    }

    size_t len = strlen(path);
    const char* end = path + len - 1;

    // Remove trailing slashes
    while (end > path && *end == '/')
    {
        --end;
    }

    // Where do we divide the current directory and the last?
    while (end > path && *end != '/')
    {
        --end;
    }

    // If we have reduced ourself to one character.
    if (end == path)
    {
        if (path[0] == '/')
        {
            return "/";
        }
        else
        {
            return ".";
        }
    }

    // Skip over the separator.
    do
    {
        --end;
    }
    while (end > path && *end == '/');

    dir.assign(path, (end - path) + 1);
    return dir;
}

bool
 realpath(const std::string& path, std::string* real)
{
    char buf[PATH_MAX + 1];
    memset(buf, 0, sizeof(buf));
    char* ret = ::realpath(path.c_str(), buf);

    if (ret == NULL)
    {
        return false;
    }

    buf[PATH_MAX] = '\0';
    *real = buf;
    return true;
}

std::string
 join(const std::string& p1,
                    const std::string& p2)
{
    if (!p2.empty() && p2[0] == '/')
    {
        return p2;
    }

    std::string ret = p1;

    if (ret == "")
    {
        ret = ".";
    }

    while (!ret.empty() && ret[ret.size() - 1] == '/')
    {
        ret.erase(ret.size() - 1);
    }

    return ret + "/" + p2;
}

std::string
 join(const std::string& p1,
                    const std::string& p2,
                    const std::string& p3)
{
    return Std::join(Std::join(p1, p2), p3);
}

std::string
 join(const std::string& p1,
                    const std::string& p2,
                    const std::string& p3,
                    const std::string& p4)
{
    return Std::join(Std::join(p1, p2, p3), p4);
}

std::string
 join(const std::string& p1,
                    const std::string& p2,
                    const std::string& p3,
                    const std::string& p4,
                    const std::string& p5)
{
    return Std::join(Std::join(p1, p2, p3, p4), p5);
}