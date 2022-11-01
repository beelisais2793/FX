// Copyright (C) 2013 Daniel Vogelbacher
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation files
// (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



#include "direntcpp.hh"

#include <dirent.h>
#include <errno.h>

#include <stdexcept>

#define DIRENTCPP_NS_BEGIN namespace direntcpp {
#define DIRENTCPP_NS_END }


DIRENTCPP_NS_BEGIN

entry::entry(struct ::dirent *de)
    : m_entry(de)
{
}

std::string
entry::name(void) const
{
    return this->m_entry->d_name;
}

    
etypes
entry::type(void) const
{
    switch(this->m_entry->d_type)
    {
    case DT_BLK: return DIRENT_BLOCK;
    case DT_CHR: return DIRENT_CHARDEV;
    case DT_DIR: return DIRENT_DIR;
    case DT_FIFO: return DIRENT_FIFO;
        //case DT_LINK: return DIRENT_LINK;
    case DT_REG: return DIRENT_FILE;
    case DT_SOCK: return DIRENT_SOCK;
    case DT_UNKNOWN:
    default:
        return DIRENT_UNKNOWN;
    }
}



dir::dir(void)
    : m_dir(NULL),
      m_entry(NULL)
{
}


dir::dir(const std::string &path)
    : m_dir(NULL),
      m_entry(NULL)
{
    this->opendir(path);
}


dir::~dir(void)
{
    this->closedir();
}

void
dir::opendir(const std::string &path)
{
    this->m_dir = ::opendir(path.c_str());
    if(!this->m_dir)
    {
        switch(errno)
        {
        case EACCES: throw std::runtime_error("EACCES");
        case EMFILE: throw std::runtime_error("EMFILE");
        case ENFILE: throw std::runtime_error("ENFILE");
        case ENOENT: throw std::runtime_error("ENOENT");
        case ENOMEM: throw std::runtime_error("ENOMEM");
        case ENOTDIR: throw std::runtime_error("ENOTDIR");
        default: throw std::runtime_error("unknown error");
        };
    }
}

void
dir::closedir(void)
{
    if(this->m_entry)
    {
        delete this->m_entry;
        this->m_entry = NULL;
    }
    if(this->m_dir)
    {
        ::closedir(this->m_dir);
        this->m_dir = 0;
    }
}

const entry*
dir::readdir(void)
{
    struct dirent *de = ::readdir(this->m_dir);
    if(de)
    {
        delete this->m_entry;
        return this->m_entry = new entry(de);
    }
    else return NULL;
}

void
dir::rewinddir(void)
{
    ::rewinddir(this->m_dir);
}

void
dir::seekdir(long loc)
{
    ::seekdir(this->m_dir, loc);
}

long
dir::telldir(void)
{
    return ::telldir(this->m_dir);
}



DIRENTCPP_NS_END

//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//