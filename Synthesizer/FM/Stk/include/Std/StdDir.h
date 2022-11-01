#pragma once

#include <dirent.h>
#include <sys/dir.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <glob.h>

namespace Std 
{
    struct Dir
    {
        DIR * dir;
        struct dirent * dent;
        size_t cnt;

        Dir(const char * path) {
            dir = opendir(path);        
            get_count();
        }
        ~Dir() {
            if(dir) closedir(dir);
        }

        long tell() const {
            return telldir(dir);
        }
        void get_count() {
            cnt = 0;
            while(dent = readdir(dir)) {            
                if( !strcmp(dent->d_name,".") || !strcmp(dent->d_name,"..")) continue;
                cnt++;
            }
            rewinddir(dir);
        }
        int  count() const { return cnt; }
        const char* entry() {
            dent = readdir(dir);
            return dent->d_name;
        }

        const char* getcwd() const {
            return getcwd();
        }
        void chdir(const char * path) {
            chdir(path);
        }
        void mkdir(const char * path) {
            mkdir(path);
        }
        void rename(const char * path, const char * new_path) {
            rename(path,new_path);
        }
        void system(const char * command) {
            system(command);
        }
    };

    struct Glob
    {
        std::vector<std::string> globs;

        Glob(const char * path) {
            glob_t globbuf;
            globbuf.gl_offs = 2;
            glob(path,GLOB_DOOFFS,NULL,&globbuf);
            std::cout << globbuf.gl_pathc << std::endl;
            for(size_t i = 0; i < globbuf.gl_pathc; i++)
                if(globbuf.gl_pathv[i])
                    globs.push_back(std::string(globbuf.gl_pathv[i]));
            globfree(&globbuf);
        }

        std::string operator[](size_t i) {
            return globs[i];
        }
        size_t size() const { return globs.size(); }
    };
}