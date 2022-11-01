#pragma once


#include "SndFile.hpp"


struct WaveFile
{
    std::vector<float> buffer;
    int cur_pos;
    bool playing;
    bool loop;

    WaveFile(const char * filename) {
        SndFileReaderFloat r(filename);
        int n = r.read(r.frames()*r.channels(),buffer);        
        cur_pos = 0;
        playing = true;
        loop = false;
    }
    float Tick()
    {
        float out = 0;
        if(playing == false) return out;
        if(cur_pos >= buffer.size()) 
            if(loop) cur_pos = 0;
            else return out;
        out = buffer[cur_pos];
        cur_pos++;
        return out;
    }
    void process(size_t n, float * buf) {
        if(playing == false) return;
        if(cur_pos >= buffer.size()) 
            if(loop) cur_pos = 0;
            else return;
        for(size_t i = cur_pos; i < cur_pos+n; i++)
        {
            if(cur_pos >= buffer.size()) break;
            buf[i-cur_pos] = 0.5*(buf[i-cur_pos] + buffer[i]);
        }
        cur_pos += n;
    }
    void stop() {
        playing = false;
    }
    void start() {
        playing = true;
    }
    void pause() {
        playing = !playing;
    }
    void reset() {
        cur_pos = 0;
    }
    void toogle_loop(bool f) {
        loop = f;
    }
};
