#pragma once 

#include "datalogic.h"

namespace Std {
    struct RegEx : public StdObject
    {
        std::regex e;

        RegEx(const std::string &pattern) : e(pattern) {} 

        bool match(const std::string & s) { return std::regex_match(s,e); }
        bool search(const std::string & s)
        {
            std::smatch m;
            return std::regex_search(s,m,e);
        }
        std::string replace(const std::string & target, const std::string & pattern)
        {
            std::string result = target;
            std::regex_replace(result,e,pattern);
            return result;
        }

    };

    // only char string for now. 
    struct String : public StdObject
    {
        std::string str;

        String() {}
        String(const char * s) { str = s; }
        String(const std::string& s) { str = s; }
        String(const String& s) { str = s.str; }

        String& operator=(const char * s) { str=s; return *this; }
        String& operator=(const std::string& s) { str=s; return *this; }    
        String& operator=(const String& s) { str=s.str; return *this; }

        String& operator+(const char * s) { str = str + s; return *this; }
        String& operator+(const std::string& s) { str = str + s; return *this; }
        String& operator+(const String& s) { str = str + s.str; return *this; }

        char& operator[](size_t index) { return str[index]; }
        char __getitem(size_t index) { return str[index]; }
        void __setitem(size_t index, const char v) { str[index] = v; }

        size_t size() { return str.size(); }
        size_t length() { return str.length(); }
        void resize(size_t r) { str.resize(r); } 
        size_t capacity() { return str.capacity(); }
        void reserve(size_t n=0) { str.reserve(n); }
        void clear() { str.clear(); }
        bool empty() { return str.empty(); }
        char& at(size_t index) { return str.at(index); }
        char& back() { return str.back(); }
        char& front() { return str.front(); }

        String cat(const char * s) { return String(str + s);  }
        String cat(const std::string& s) { return String(str + s); }
        String cat(const String& s) { return String(str + s.str); }

        String& append(const char * s) { str.append(s); return *this; }
        String& append(const std::string& s) { str.append(s); return *this; }
        String& append(const String& s) { str.append(s.str); return *this; }

        void push_back(char c) { str.push_back(c); }
        void pop_back() { str.pop_back(); }

        void assign(const String & s) { str.assign(s.str); }
        void assign(const char* s) { str.assign(s); }
        void assign(const std::string& s) { str.assign(s); }

        void assign(const String & s,size_t subpos, size_t sublen) { str.assign(s.str,subpos,sublen); }
        void assign(const char * s,size_t subpos, size_t sublen) { str.assign(s,subpos,sublen); }
        void assign(const std::string& s,size_t subpos, size_t sublen) { str.assign(s,subpos,sublen); }

        String insert(size_t pos, const String&r) { return String(str.insert(pos,r.str));  }
        String insert(size_t pos, const std::string& r) { return String(str.insert(pos,r));  }
        String insert(size_t pos, const char* r) { return String(str.insert(pos,r));  }
        String insert(size_t pos, const char* r, size_t n) { return String(str.insert(pos,r,n));  }
        String insert(size_t pos, size_t n, char c) { return String(str.insert(pos,n,c));  }

        String& erase(size_t pos=0, size_t len=std::string::npos) { str.erase(pos,len); return *this; }

        String replace(size_t pos, size_t len, const String & r) { return String(str.replace(pos,len,r.str)); }    
        String replace(size_t pos, size_t len, const String & r,size_t subpos, size_t sublen) { return String(str.replace(pos,len,r.str,subpos,sublen)); }    
        String replace(size_t pos, size_t len, const std::string& r) { return String(str.replace(pos,len,r)); }
        String replace(size_t pos, size_t len, const char* r) { return String(str.replace(pos,len,r)); }
        String replace(size_t pos, size_t len, const char* r,size_t n) { return String(str.replace(pos,len,r,n)); }
        
        void swap(String & s) { str.swap(s.str); }
        void swap(std::string & s) { str.swap(s); }

        const char* c_str() { return str.c_str(); }
        const char* data() { return str.data(); }

        size_t copy(char * s, size_t len, size_t pos) { return str.copy(s,len,pos); }

        size_t find(const String& s, size_t pos = 0) { return str.find(s.str,pos); }
        size_t find(const std::string& s, size_t pos = 0) { return str.find(s,pos); }
        size_t find(const char* s, size_t pos = 0) { return str.find(s,pos); }
        size_t find(const char* s, size_t pos, size_t n) { return str.find(s,pos,n); }
        size_t find(char c, size_t pos = 0) { return str.find(c,pos); }

        size_t rfind(const String& s, size_t pos = 0) { return str.rfind(s.str,pos); }
        size_t rfind(const std::string& s, size_t pos = 0) { return str.rfind(s,pos); }
        size_t rfind(const char* s, size_t pos = 0) { return str.rfind(s,pos); }
        size_t rfind(const char* s, size_t pos, size_t n) { return str.rfind(s,pos,n); }
        size_t rfind(char c, size_t pos = 0) { return str.rfind(c,pos); }

        size_t find_first_of(const String& s, size_t pos = 0) { return str.find_first_of(s.str,pos); }
        size_t find_first_of(const std::string& s, size_t pos = 0) { return str.find_first_of(s,pos); }
        size_t find_first_of(const char* s, size_t pos = 0) { return str.find_first_of(s,pos); }
        size_t find_first_of(const char* s, size_t pos, size_t n) { return str.find_first_of(s,pos,n); }
        size_t find_first_of(char c, size_t pos = 0) { return str.find_first_of(c,pos); }

        size_t find_last_of(const String& s, size_t pos = 0) { return str.find_last_of(s.str,pos); }
        size_t find_last_of(const std::string& s, size_t pos = 0) { return str.find_last_of(s,pos); }
        size_t find_last_of(const char* s, size_t pos = 0) { return str.find_last_of(s,pos); }
        size_t find_last_of(const char* s, size_t pos, size_t n) { return str.find_last_of(s,pos,n); }
        size_t find_last_of(char c, size_t pos = 0) { return str.find_last_of(c,pos); }

        size_t find_first_not_of(const String& s, size_t pos = 0) { return str.find_first_not_of(s.str,pos); }
        size_t find_first_not_of(const std::string& s, size_t pos = 0) { return str.find_first_not_of(s,pos); }
        size_t find_first_not_of(const char* s, size_t pos = 0) { return str.find_first_not_of(s,pos); }
        size_t find_first_not_of(const char* s, size_t pos, size_t n) { return str.find_first_not_of(s,pos,n); }
        size_t find_first_not_of(char c, size_t pos = 0) { return str.find_first_not_of(c,pos); }

        size_t find_last_not_of(const String& s, size_t pos = 0) { return str.find_last_not_of(s.str,pos); }
        size_t find_last_not_of(const std::string& s, size_t pos = 0) { return str.find_last_not_of(s,pos); }
        size_t find_last_not_of(const char* s, size_t pos = 0) { return str.find_last_not_of(s,pos); }
        size_t find_last_not_of(const char* s, size_t pos, size_t n) { return str.find_last_not_of(s,pos,n); }
        size_t find_last_not_of(char c, size_t pos = 0) { return str.find_last_not_of(c,pos); }

        String substr(size_t pos = 0, size_t len = std::string::npos) { return String(str.substr(pos,len)); }

        int compare(const String& s) { return str.compare(s.str); }
        int compare(const std::string& s) { return str.compare(s); }
        

        bool operator == (const String& b) { return str == b.str; }
        bool operator == (const std::string& b) { return str == b; }
        bool operator == (const char* b) { return str == b; }

        void fill(char c)
        {
            std::fill(str.begin(),str.end(), c);
        }
        void reverse() { std::reverse(str.begin(),str.end()); }
        void random_shuffle() { std::random_shuffle(str.begin(), str.end()); }
    };
}
