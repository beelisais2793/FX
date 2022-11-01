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




#include <string>
#include <vector>

#include <dirent.h>



namespace direntcpp
{


class dir;
class entry;


enum etypes
{
    DIRENT_BLOCK,
    DIRENT_CHARDEV,
    DIRENT_DIR,
    DIRENT_FIFO,
    DIRENT_LINK,
    DIRENT_FILE,
    DIRENT_SOCK,
    DIRENT_UNKNOWN
};


class entry
{
public:
	entry(struct ::dirent *de);
	std::string name(void) const;
    etypes type(void) const;
    

private:
	struct ::dirent *m_entry;

    entry(const entry &);
    entry& operator=(const entry &);
};


class dir
{
public:
	dir(void);
	dir(const std::string &path);
	~dir(void);

	void opendir(const std::string &path);

	void closedir(void);

	const entry* readdir(void);

	void rewinddir(void);

	void seekdir(long loc);

	long telldir(void);

private:
	::DIR *m_dir;
	entry *m_entry;

    dir(const dir &);
    dir& operator=(const dir &);
};


}


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
