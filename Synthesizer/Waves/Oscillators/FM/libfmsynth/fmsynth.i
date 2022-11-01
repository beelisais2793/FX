%module fmsynth
%{
#include <fmsynth.h>
#include <cassert>
#include <vector>
%}
%include "stdint.i"
%include "std_vector.i"

%template(float_vector) std::vector<float>;
%template(dbl_vector) std::vector<double>;

%include <fmsynth.h>

%inline %{

    void print_parameters() {

        static const char* params[] = {
            "FMSYNTH_PARAM_AMP",
            "FMSYNTH_PARAM_PAN",
            "FMSYNTH_PARAM_FREQ_MOD",
            "FMSYNTH_PARAM_FREQ_OFFSET",
            "FMSYNTH_PARAM_ENVELOPE_TARGET0",
            "FMSYNTH_PARAM_ENVELOPE_TARGET1",
            "FMSYNTH_PARAM_ENVELOPE_TARGET2",
            "FMSYNTH_PARAM_DELAY0",
            "FMSYNTH_PARAM_DELAY1",
            "FMSYNTH_PARAM_DELAY2",
            "FMSYNTH_PARAM_RELEASE_TIME",
            "FMSYNTH_PARAM_KEYBOARD_SCALING_MID_POINT",
            "FMSYNTH_PARAM_KEYBOARD_SCALING_LOW_FACTOR",
            "FMSYNTH_PARAM_KEYBOARD_SCALING_HIGH_FACTOR",                                                       
            "FMSYNTH_PARAM_VELOCITY_SENSITIVITY",
            "FMSYNTH_PARAM_MOD_WHEEL_SENSITIVITY",
            "FMSYNTH_PARAM_LFO_AMP_SENSITIVITY",
            "FMSYNTH_PARAM_LFO_FREQ_MOD_DEPTH",
            "FMSYNTH_PARAM_ENABLE",
            "FMSYNTH_PARAM_CARRIERS",
            "FMSYNTH_PARAM_MOD_TO_CARRIERS0",
            "FMSYNTH_PARAM_END",
            "FMSYNTH_PARAM_ENSURE_INT",
            0};

        const char* p = params[0];
        while(*p != 0)
        {
            std::cout << p << std::endl;
            p++;
        }
    }

    void print_global_parameters() {
        static const char * params[] = {
        "FMSYNTH_GLOBAL_PARAM_VOLUME",
        "FMSYNTH_GLOBAL_PARAM_LFO_FREQ",
        "FMSYNTH_GLOBAL_PARAM_END",
        "FMSYNTH_GLOBAL_PARAM_ENSURE_INT",
        0};
        const char * p = params[0];
        while(*p) {
            std::cout << p << std::endl;
            p++;
        }
    }

    void print_status() {
        static const char* params[] = {
            "FMSYNTH_STATUS_OK",
            "FMSYNTH_STATUS_BUSY",
            "FMSYNTH_STATUS_BUFFER_TOO_SMALL",
            "FMSYNTH_STATUS_NO_NUL_TERMINATE",
            "FMSYNTH_STATUS_INVALID_FORMAT",
            "FMSYNTH_STATUS_MESSAGE_UNKNOWN",
            "FMSYNTH_STATUS_ENSURE_INT",
            0};
        const char * p = params[0];
        while(*p) {
            std::cout << p << std::endl;
            p++;
        }        
    }
    struct FMPreset
    {        
        uint8_t *buffer;
        size_t   size;

        FMPreset(const char * filename) {
            load(filename);
        }
        FMPreset(size_t size) {
            buffer = (uint8_t*)malloc(size);
            assert(buffer != NULL);
            this->size = size;
        }
        ~FMPreset() {
            if(buffer) free(buffer);
        }
        uint8_t __getitem__(size_t i) { return buffer[i]; }
        void    __setitem__(size_t i, uint8_t val) { buffer[i] = val; }

        void save(const char * filename)
        {
            FILE * f = fopen(filename,"wb");
            fwrite(&size,sizeof(size_t),1,f);
            fwrite(buffer,sizeof(uint8_t*),size,f);
            fclose(f);
        }
        void load(const char * filename) {
            if(buffer) free(buffer);
            FILE * f = fopen(filename,"rb");
            fread(&size,sizeof(size_t),1,f);
            buffer = (uint8_t*)malloc(size);
            fread(buffer,size,sizeof(uint8_t),f);
            fclose(f);
        }        
    };

    struct FMSynth
    {
        fmsynth_t * fm;

        FMSynth(float sr, unsigned max_voices) {
            fm = fmsynth_new(sr,max_voices);
            assert(fm != nullptr);
        }
        ~FMSynth() {
            if(fm) fmsynth_free(fm);
        }

        void reset() { fmsynth_reset(fm); }

        void set_parameter(unsigned parameter, unsigned index, float val) {
            fmsynth_set_parameter(fm,parameter,index,val);
        }
        float get_parameter(unsigned parameter, unsigned index) {
            return fmsynth_get_parameter(fm,parameter,index);
        }
        float convert_from_normalized_parameter(unsigned parameter, float val) {
            return fmsynth_convert_from_normalized_parameter(fm,parameter,val);
        }
        float convert_to_normalized_parameter(unsigned parameter, float val) {
            return fmsynth_convert_to_normalized_parameter(fm,parameter,val);
        }
        void set_global_parameter(unsigned param, float val) {
            fmsynth_set_global_parameter(fm,param,val);
        }
        float get_global_parameter(unsigned param) {
            return fmsynth_get_global_parameter(fm,param);
        }
        float convert_from_normalized_global_parameter(unsigned parameter, float val) {
            return fmsynth_convert_from_normalized_global_parameter(fm,parameter,val);
        }
        float convert_to_normalized_global_parameter(unsigned parameter, float val) {
            return fmsynth_convert_to_normalized_global_parameter(fm,parameter,val);
        }

    
        fmsynth_status_t save_preset(FMPreset & buffer) {
            return fmsynth_preset_save(fm,NULL, buffer.buffer, fmsynth_preset_size());
        }
        fmsynth_status_t load_preset(FMPreset & buffer) {
            return fmsynth_preset_load(fm,NULL,buffer.buffer,fmsynth_preset_size());
        }

        unsigned render(float * left, float * right, unsigned frames) {
            return fmsynth_render(fm,left,right,frames);
        }
        fmsynth_status_t note_on(uint8_t note, uint8_t vel) {
            return fmsynth_note_on(fm,note,vel);
        }
        void note_off(uint8_t note) { fmsynth_note_off(fm,note); }
        void set_sustain(bool enable) { fmsynth_set_sustain(fm,enable); }
        void set_mod_wheel(uint8_t wheel) { fmsynth_set_mod_wheel(fm,wheel); }
        void set_pitch_bend(uint16_t val) { fmsynth_set_pitch_bend(fm,val); }
        void release_all() { fmsynth_release_all(fm); }
    };
%}