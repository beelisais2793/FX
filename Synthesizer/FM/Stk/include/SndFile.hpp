#pragma once 

extern "C" {
#include <sndfile.h>
#include <stdint.h>
}

#include <vector>
#include <cassert>
#include <cstring>

namespace SoundWave {


    struct SndFileReader
    {
        SNDFILE * sndfile;
        SF_INFO   info;

        SndFileReader(const char * filename) {
            sndfile = sf_open(filename,SFM_READ,&info);
            assert(sndfile != NULL);
        }
        ~SndFileReader() {
            if(sndfile) sf_close(sndfile);
        }    

        int64_t size() const {
            sf_count_t x = sf_seek(sndfile,0,SEEK_END);
            sf_seek(sndfile,0,SEEK_SET);
            return x;
        }
        
        SndFileReader& operator >> (std::vector<float> & s) {
            sf_read_float(sndfile,s.data(),s.size());
            return *this;
        }
        int64_t read(size_t n, std::vector<float> & samples) {        
            samples.resize(n);
            return sf_read_float(sndfile,samples.data(),n);
        }
        int64_t read(size_t n, float * samples) {                    
            return sf_read_float(sndfile,samples,n);
        }

        SndFileReader& operator >> (std::vector<double> & s) {
            sf_read_double(sndfile,s.data(),s.size());
            return *this;
        }
        int64_t read(size_t n, std::vector<double> & samples) {        
            samples.resize(n);
            return sf_read_double(sndfile,samples.data(),n);
        }
        int64_t read(size_t n, double * samples) {                    
            return sf_read_double(sndfile,samples,n);
        }

        SndFileReader& operator >> (std::vector<short> & s) {
            sf_read_short(sndfile,s.data(),s.size());
            return *this;
        }
        int64_t read(size_t n, std::vector<short> & samples) {        
            samples.resize(n);
            return sf_read_short(sndfile,samples.data(),n);
        }
        int64_t read(size_t n, short * samples) {                    
            return sf_read_short(sndfile,samples,n);
        }

        SndFileReader& operator >> (std::vector<int> & s) {
            sf_read_int(sndfile,s.data(),s.size());
            return *this;
        }
        int64_t read(size_t n, std::vector<int> & samples) {        
            samples.resize(n);
            return sf_read_int(sndfile,samples.data(),n);
        }
        int64_t read(size_t n, int * samples) {                    
            return sf_read_int(sndfile,samples,n);
        }


        int64_t read_frames(size_t n, std::vector<float> & samples) {        
            samples.resize(n);
            return sf_readf_float(sndfile,samples.data(),n);
        }
        int64_t read_frames(size_t n, float * samples) {                    
            return sf_readf_float(sndfile,samples,n);
        }

        int64_t read_frames(size_t n, std::vector<double> & samples) {        
            samples.resize(n);
            return sf_readf_double(sndfile,samples.data(),n);
        }
        int64_t read_frames(size_t n, double * samples) {                    
            return sf_readf_double(sndfile,samples,n);
        }

        int64_t read_frames(size_t n, std::vector<short> & samples) {        
            samples.resize(n);
            return sf_readf_short(sndfile,samples.data(),n);
        }
        int64_t read_frames(size_t n, short * samples) {                    
            return sf_readf_short(sndfile,samples,n);
        }

        int64_t read_frames(size_t n, std::vector<int> & samples) {        
            samples.resize(n);
            return sf_readf_int(sndfile,samples.data(),n);
        }
        int64_t read_frames(size_t n, int * samples) {                    
            return sf_readf_int(sndfile,samples,n);
        }

        int64_t seek(int64_t frames, int whence = SEEK_SET) {
            return sf_seek(sndfile,frames,whence);
        }

        const char* get_string(int str_type) {
            return sf_get_string(sndfile,str_type);
        }
        int command(int cmd, void * data, int datasize) {
            return sf_command(sndfile,cmd,data,datasize);
        }

        int samplerate() const { return info.samplerate; }
        int channels() const { return info.channels; }
        int format() const { return info.format;}
        int sections() const { return info.sections; }
        int seekable() const { return info.seekable; }
        
    };

    struct SndFileWriter
    {
        SNDFILE * sndfile;
        SF_INFO   info;

        SndFileWriter(const char * filename, int format, int channels, int sample_rate) {
            memset(&info,0x00,sizeof(info));
            info.channels = channels;
            info.format = format;
            info.samplerate = sample_rate;
            sndfile = sf_open(filename,SFM_WRITE,&info);
        }
        ~SndFileWriter() {
            if(sndfile) sf_close(sndfile);
        }
        void close() {
            if(sndfile) {
                sf_close(sndfile);
                sndfile = NULL;
            }
        }

        SndFileWriter& operator << (std::vector<float> & s) {
            sf_write_float(sndfile,s.data(),s.size());
            return *this;
        }
        void write(std::vector<float> & v) {
            sf_write_float(sndfile,v.data(),v.size());
        }
        void write(size_t n,float * v) {
            sf_write_float(sndfile,v,n);
        }

        SndFileWriter& operator << (std::vector<double> & s) {
            sf_write_double(sndfile,s.data(),s.size());
            return *this;
        }
        void write(std::vector<double> & v) {
            sf_write_double(sndfile,v.data(),v.size());
        }
        void write(size_t n,double * v) {
            sf_write_double(sndfile,v,n);
        }

        SndFileWriter& operator << (std::vector<short> & s) {
            sf_write_short(sndfile,s.data(),s.size());
            return *this;
        }
        void write(std::vector<short> & v) {
            sf_write_short(sndfile,v.data(),v.size());
        }
        void write(size_t n,short * v) {
            sf_write_short(sndfile,v,n);
        }


        SndFileWriter& operator << (std::vector<int> & s) {
            sf_write_int(sndfile,s.data(),s.size());
            return *this;
        }
        void write(std::vector<int> & v) {
            sf_write_int(sndfile,v.data(),v.size());
        }
        void write(size_t n,int * v) {
            sf_write_int(sndfile,v,n);
        }


        void write_frames(std::vector<float> & v) {
            sf_writef_float(sndfile,v.data(),v.size());
        }
        void write_frames(size_t n, float * v) {
            sf_writef_float(sndfile,v,n);
        }

        void write_frames(std::vector<double> & v) {
            sf_writef_double(sndfile,v.data(),v.size());
        }
        void write_frames(size_t n,double * v) {
            sf_writef_double(sndfile,v,n);
        }

        void write_frames(std::vector<short> & v) {
            sf_writef_short(sndfile,v.data(),v.size());
        }
        void write_frames(size_t n,short * v) {
            sf_writef_short(sndfile,v,n);
        }

        void write_frames(std::vector<int> & v) {
            sf_writef_int(sndfile,v.data(),v.size());
        }
        void write_frames(size_t n,int * v) {
            sf_writef_int(sndfile,v,n);
        }

        
        int64_t seek(int64_t frames, int whence = SEEK_SET) {
            return sf_seek(sndfile,frames,whence);
        }
        int set_string(int str_type, const char * str) {
            return sf_set_string(sndfile,str_type,str);
        }
        void sync() {
            sf_write_sync(sndfile);
        }
        
        int command(int cmd, void * data, int datasize) {
            return sf_command(sndfile,cmd,data,datasize);
        }

        int samplerate() const { return info.samplerate; }
        int channels() const { return info.channels; }
        int format() const { return info.format;}
        int sections() const { return info.sections; }
        int seekable() const { return info.seekable; }

    };
};