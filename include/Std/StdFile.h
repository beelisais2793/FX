#pragma once 

#include "StdObject.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Std
{
 
 enum SeekType
 {
     BEGIN = SEEK_SET,
     CURRENT= SEEK_CUR,
     END = SEEK_END,
 };

 class StdFile : public StdObject 
 {
    protected:

        FILE * file;

    public:

    
        StdFile(const char * filename, const char * mode) {
            file = fopen(filename,mode);
            assert(file != NULL);
        }
        ~StdFile() { if(file) fclose(file); }

        void open(const char * filename, const char * mode) {
            if(file) fclose(file);
            file = fopen(filename,mode);
            assert(file != NULL);
        }
        void close() { if(file) { fclose(file); file = NULL;} }

        void seek(uint64_t pos, SeekType w = BEGIN) {
            fseek(file,pos,w);
        }

    };

    class BinaryFileWriter : public StdFile
    {
    public:

        BinaryFileWriter(const char * file, const char * mode = "wb") : StdFile(file,mode) {        
        }
        
        void write(int8_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(int16_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(int32_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(int64_t i) { fwrite(&i,sizeof(i),1,file); }        
        void write(uint8_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(uint16_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(uint32_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(uint64_t i) { fwrite(&i,sizeof(i),1,file); }
        void write(const char * s) {
            size_t len = strlen(s);
            write(len);
            fwrite(s,1,strlen(s),file);
        }
        void write(const std::string& s) {
            write(s.data());
        }

        template<typename T>
        void write(const std::vector<T> & v)
        {
            size_t len = v.size();
            write(len);
            fwrite(v.data(),sizeof(T),v.size(),file);
        }
        template<typename T>
        void write(const StdVector<T> & v)
        {
            size_t len = v.size();
            write(len);
            fwrite(v.vector.data(),sizeof(T),v.size(),file);
        }


        BinaryFileWriter& operator << (int8_t i) { fwrite(&i,sizeof(i),1,file); return *this; }
        BinaryFileWriter& operator << (int16_t i) { fwrite(&i,sizeof(i),1,file); return *this;}
        BinaryFileWriter& operator << (int32_t i) { fwrite(&i,sizeof(i),1,file); return *this; }
        BinaryFileWriter& operator << (int64_t i) { fwrite(&i,sizeof(i),1,file); return *this; }
        BinaryFileWriter& operator << (uint8_t i) { fwrite(&i,sizeof(i),1,file); return *this;}
        BinaryFileWriter& operator << (uint16_t i) { fwrite(&i,sizeof(i),1,file); return *this;}
        BinaryFileWriter& operator << (uint32_t i) { fwrite(&i,sizeof(i),1,file); return *this;}
        BinaryFileWriter& operator << (uint64_t i) { fwrite(&i,sizeof(i),1,file); return *this;}
        BinaryFileWriter& operator << (float i) { fwrite(&i,sizeof(i),1,file); return *this;}            
        BinaryFileWriter& operator << (double i) { fwrite(&i,sizeof(i),1,file); return *this;}
        BinaryFileWriter& operator << (const StdString& i) { 
            size_t n = i.size();
            fwrite(&n,sizeof(n),1,file);
            fwrite(i.data(),sizeof(char),n,file); 
            return *this;
        }

    };

    class BinaryFileReader : public StdFile
    {
    public:

        BinaryFileReader(const char * file, const char * mode = "rb") : StdFile(file,mode) {        
        }

        
        size_t read(int8_t *i)  { return fread(i,sizeof(i),1,file); }
        size_t read(int16_t *i) { return fread(i,sizeof(i),1,file); }
        size_t read(int32_t *i) { return fread(i,sizeof(i),1,file); }
        size_t read(int64_t *i) { return fread(i,sizeof(i),1,file); }        
        size_t read(uint8_t *i) { return fread(i,sizeof(i),1,file); }
        size_t read(uint16_t *i) { return fread(i,sizeof(i),1,file); }
        size_t read(uint32_t *i) { return fread(i,sizeof(i),1,file); }
        size_t read(uint64_t *i) { return fread(i,sizeof(i),1,file); }
        
        std::string read() {
            size_t n=0;
            std::string s;
            read(&n);
            if(n > 0) {
                s.resize(n+1);
                memset(s.data(),0,n+1);
                fread(s.data(),1,n,file);
            }
            return s;
        }
        
        template<typename T>
        size_t read(std::vector<T> & v)
        {
            size_t n = 0;
            read(&n);            
            if(n > 0) {
                v.resize(n);
                n = fread(v.data(),sizeof(T),v.size(),file);
            }
            return n;
        }
        template<typename T>
        size_t read(StdVector<T> & v)
        {
            size_t n = 0;
            read(&n);
            std::cout << n << std::endl;
            if(n > 0) {
                v.resize(n);
                n = fread(v.vector.data(),sizeof(T),v.size(),file);
            }
            return n;
        }

        BinaryFileReader& operator >> (int8_t &i) { fread(&i,sizeof(i),1,file); return *this; }
        BinaryFileReader& operator >> (int16_t &i) { fread(&i,sizeof(i),1,file); return *this;}
        BinaryFileReader& operator >> (int32_t &i) { fread(&i,sizeof(i),1,file); return *this; }
        BinaryFileReader& operator >> (int64_t &i) { fread(&i,sizeof(i),1,file); return *this; }
        BinaryFileReader& operator >> (uint8_t &i) { fread(&i,sizeof(i),1,file); return *this;}
        BinaryFileReader& operator >> (uint16_t &i) { fread(&i,sizeof(i),1,file); return *this;}
        BinaryFileReader& operator >> (uint32_t &i) { fread(&i,sizeof(i),1,file); return *this;}
        BinaryFileReader& operator >> (uint64_t &i) { fread(&i,sizeof(i),1,file); return *this;}
        BinaryFileReader& operator >> (float &i) { fread(&i,sizeof(i),1,file); return *this;}    
        BinaryFileReader& operator >> (double &i) { fread(&i,sizeof(i),1,file); return *this;}        
        BinaryFileReader& operator >> (const StdString& i) { 
            size_t n = i.size();
            fread(&n,sizeof(n),1,file);
            fread((void*)i.data(),sizeof(char),n,file); 
            return *this;
        }

    };
    }


        
        
        