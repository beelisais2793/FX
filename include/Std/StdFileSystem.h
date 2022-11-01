#pragma once

#include "datalogic.h"


namespace Std 
{
    using string = std::string;

    struct Path
    {
    private:
        std::filesystem::path path; 

    public:
        Path() {}
        Path(const std::string p) : path(p) {} 
        Path(const Path& p) : path(p.path) {}
        
        Path& operator = (const Path &p) { path = p.path; return *this; }

        void clear() { path.clear(); }
        void make_preferred() { path.make_preferred(); }
        Path remove_filename() { return Path(path.remove_filename()); } 
        Path replace_filename(const Path& r) { return Path(path.replace_filename(r.path)); }        
        Path replace_extension( const Path& r) { return Path(path.replace_extension(r.path)); }
        void swap( Path & other) { path.swap(other.path); }

        const char* c_str() const { return path.c_str(); }
        std::string string() const { return path.string(); }
        std::string genetic_string() const { return path.generic_string(); }
        int compare(const Path & p) { return path.compare(p.path); }
        Path lexically_normal() const { return Path(path.lexically_normal()); }
        Path lexically_relative(const Path &p) const { return Path(path.lexically_relative(p.path)); }
        Path lexically_proximate(const Path &p) const { return Path(path.lexically_proximate(p.path)); }

        Path root_name() const { return Path(path.root_name()); }
        Path root_directory() const { return Path(path.root_directory()); }
        Path root_path() const { return Path(path.root_path()); }
        Path relative_path() const { return Path(path.relative_path()); }
        Path parent_path() const { return Path(path.parent_path()); }
        Path filename() const { return Path(path.filename()); }
        Path stem() const { return Path(path.stem()); }
        Path extension() const { return Path(path.extension()); }

        bool empty() { return path.empty(); }
        bool has_root_path() const { return path.has_root_path(); }
        bool has_root_name() const { return path.has_root_name();}	
        bool has_root_directory() const { return path.has_root_directory(); }
        bool has_relative_path() const { return path.has_relative_path(); }
        bool has_parent_path() const { return path.has_parent_path(); }
        bool has_filename() const { return path.has_filename(); }
        bool has_stem() const { return path.has_stem(); }
        bool has_extension() const { return path.has_extension(); }
		bool is_absolute() const { return path.is_absolute(); }
        bool is_relative() const { return path.is_relative(); }
        
        bool operator == (const Path &p) { return path == p.path; }
        bool operator != (const Path &p) { return path != p.path; }
        bool operator <= (const Path &p) { return path <= p.path; }
        bool operator >= (const Path &p) { return path >= p.path; }
        bool operator < (const Path &p) { return path < p.path; }
        bool operator > (const Path &p) { return path > p.path; }

        void concat(const Path & p) { path = path / p.path; }
    };
    
    struct DirEntry 
    {
    private:
        std::filesystem::directory_entry entry;
    public:
        DirEntry() {}
        DirEntry(std::filesystem::directory_entry & e) : entry(e) {} 

        DirEntry& operator=(const DirEntry& e) { entry = e.entry; return *this; }

        std::string path() { return entry.path().string(); }
        Path to_path() { return Path(entry.path()); }

        bool exists() const { return entry.exists(); }        

        bool is_block_file() { return entry.is_block_file(); }
        bool is_character_file() { return entry.is_character_file(); }
        bool is_directory() { return entry.is_directory(); }        
        bool is_fifo() { return entry.is_fifo(); }
        bool is_other() { return entry.is_other(); }
        bool is_regular_file() { return entry.is_regular_file(); }
        bool is_socket() { return entry.is_socket(); }
        bool is_symlink() { return entry.is_symlink(); }
      
        uintmax_t hard_link_count() { return entry.hard_link_count(); }

        int type() const { 
            std::filesystem::file_status status = entry.status(); 
            return static_cast<int>(status.type()); 
        }
        int permissions() const { 
            std::filesystem::file_status status = entry.status(); 
            return static_cast<int>(status.permissions()); 
        }
    };
   
    struct FileStatus
    {
    private:
        std::filesystem::file_status status;
    public:
        FileStatus(std::filesystem::file_status s) : status(s) {} 

        FileStatus& operator = (const FileStatus & s) { status = s.status; return *this; }
        //bool operator == (const FileStatus & s) { return status == s.status; }

        int type() const { return static_cast<int>(status.type()); }
        int permissions() const { return static_cast<int>(status.permissions()); }

    };

    struct FileSystem
    {            
        std::string absolute(const std::string& p) { return std::filesystem::absolute(p).string(); }
        std::string canonical(const std::string& p) { return std::filesystem::canonical(p).string(); }
        std::string weakly_canonical(const std::string& p) { return std::filesystem::weakly_canonical(p).string(); }
        std::string relative(const std::string& p) { return std::filesystem::relative(p).string(); }
        std::string proximate(const std::string & p) { return std::filesystem::proximate(p).string(); }
        
        
        void copy(const std::string & from, const std::string & to) { std::filesystem::copy(from, to); }
        void copy_file(const std::string & from, const std::string & to) { std::filesystem::copy_file(from, to); }
        void copy_symlink(const std::string & from, const std::string & to) { std::filesystem::copy_symlink(from, to); }
        
        bool create_directory(const std::string & path) { return std::filesystem::create_directory(path); }
        bool create_directories(const std::string & path) { return std::filesystem::create_directories(path); }

        void create_hard_link(const std::string & target, const std::string & link) { std::filesystem::create_hard_link(target,link); }
        void create_symlink(const std::string & target, const std::string & link) { std::filesystem::create_symlink(target,link); }
        void create_directory_symlink(const std::string & target, const std::string & link) { std::filesystem::create_directory_symlink(target,link); }

        std::string current_path() { return std::filesystem::current_path(); }
        bool exists(const std::string & p) { return std::filesystem::exists(p); }
        bool equivalent(const std::string & p1, const std::string & p2) { return std::filesystem::equivalent(p1,p2); }

        uintmax_t file_size(const std::string & p) { return std::filesystem::file_size(p); }
        uintmax_t hard_link_count(const std::string & p) { return std::filesystem::hard_link_count(p); }
        
        /*
        std::string last_write_time(const std::string & p)
        {
            std::filesystem::file_time_type ftime = std::filesystem::last_write_time(p);
            std::time_t cftime = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::to_sys(ftime));
            return std::asctime(std::localtime(&cftime));
        }
        */
        std::string read_symlink(const std::string & p) { return std::filesystem::read_symlink(p).string(); }

        bool remove(const std::string & p) { return std::filesystem::remove(p); }
        uintmax_t remove_all(const std::string & p) { return std::filesystem::remove_all(p); }        
        void rename(const std::string & old, const std::string &newf) { std::filesystem::rename(old,newf); }

        void resize_file(const std::string &p, uintmax_t size) { std::filesystem::resize_file(p, size); }
        std::string temp_directory_path() { return std::filesystem::temp_directory_path().string(); }

        bool is_block_file(const std::string &p) { return std::filesystem::is_block_file(p); }
        bool is_character_file(const std::string &p) { return std::filesystem::is_character_file(p); }
        bool is_directory(const std::string &p) { return std::filesystem::is_directory(p); }
        bool is_empty(const std::string &p) { return std::filesystem::is_empty(p); }
        bool is_fifo(const std::string &p) { return std::filesystem::is_fifo(p); }
        bool is_other(const std::string &p) { return std::filesystem::is_other(p); }
        bool is_regular_file(const std::string &p) { return std::filesystem::is_regular_file(p); }
        bool is_socket(const std::string &p) { return std::filesystem::is_socket(p); }
        bool is_symlink(const std::string &p) { return std::filesystem::is_symlink(p); }

        std::vector<std::string>& dir(const std::string &p, std::vector<std::string> & r)
        {            
            std::filesystem::path d(p);
            r.clear();
            
            for(const auto & dir_entry: std::filesystem::directory_iterator{d})
            {                
                r.push_back(dir_entry.path().string());
            }
            return r;
        }
        std::vector<std::string>& rdir(const std::string &p, std::vector<std::string> & r)
        {            
            std::filesystem::path d(p);
            r.clear();
            
            for(const auto & dir_entry: std::filesystem::recursive_directory_iterator{d})
            {                
                r.push_back(dir_entry.path().string());
            }
            return r;
        }

    };
};

