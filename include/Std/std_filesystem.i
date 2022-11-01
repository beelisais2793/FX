%{
#include <filesystem>
%}

namespace std::filesystem {
  
    class path
    {
    public:    

        path();
        path(const path& );
        path(std::string& source, format fmt = auto_format);
        ~paht();

        path& operator = (const path &p);

        void clear();
        void make_preferred();
        path remove_filename();
        path replace_filename(const path& r);
        path replace_extension( const path& r);
        void swap( path & other);

        const char* c_str() const ;
        std::string string() const ;        
        int compare(const path & p);
        path lexically_normal() const;
        path lexically_relative(const path &p) ;
        path lexically_proximate(const Path &p) ;

        path root_name() const;
        path root_directory() const;
        path root_path() const;
        path relative_path() const;
        path parent_path() const;
        path filename() const;
        path stem() const;
        path extension() const;

        bool empty();
        bool has_root_path() const;
        bool has_root_name() const;
        bool has_root_directory() const;
        bool has_relative_path() const;
        bool has_parent_path() const;
        bool has_filename() const;
        bool has_stem() const;
        bool has_extension() const;
		bool is_absolute() const;
        bool is_relative() const;
        
        bool operator == (const path &p);
        bool operator != (const path &p);
        bool operator <= (const path &p);
        bool operator >= (const path &p);
        bool operator < (const path &p);
        bool operator > (const path &p);

        void concat(const Path & p);
    };
    class filesystem_error
    {
    public:
        filesystem_err(const std::string & what_arg, std::error_code ec);
        filesystem_err(const std::string & what_arg, const std::filesystem::path &p1, std::error_code ec);
        
        
    }
    class directory_entry
    {

        directory_entry& operator=(const directory_entry& e);

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
}