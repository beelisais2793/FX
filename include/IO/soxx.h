#pragma once 

#include <libsox.h>


enum EncodingType {
    ENC_UNKNOWN = SOX_ENCODING_UNKNOWN,
    ENC_SIGN2   = SOX_ENCODING_SIGN2,
    ENC_UNSIGNED = SOX_ENCODING_UNSIGNED,
    ENC_FLOAT = SOX_ENCODING_FLOAT,
    ENC_FLOAT_TEXT = SOX_ENCODING_FLOAT_TEXT,
    ENC_FLAC = SOX_ENCODING_FLAC,
    ENC_HCOM = SOX_ENCODING_HCOM,
    ENC_WAVPACK = SOX_ENCODING_WAVPACK,
    ENC_WAVPACKF = SOX_ENCODING_WAVPACKF,
    ENC_ULAW = SOX_ENCODING_ULAW,
    ENC_ALAW = SOX_ENCODING_ALAW,
    ENC_G721 = SOX_ENCODING_G721,
    ENC_G723 = SOX_ENCODING_G723,
    ENC_CL_ADPCM = SOX_ENCODING_CL_ADPCM,
    ENC_CL_ADPCM16 = SOX_ENCODING_CL_ADPCM16,
    ENC_MS_ADPCM = SOX_ENCODING_MS_ADPCM,
    ENC_IMA_ADPCM = SOX_ENCODING_IMA_ADPCM,
    ENC_OKI_ADPCM = SOX_ENCODING_OKI_ADPCM,
    ENC_DPCM = SOX_ENCODING_DPCM,
    ENC_DWVW = SOX_ENCODING_DWVW,
    ENC_DWVWN = SOX_ENCODING_DWVWN,
    ENC_GSM = SOX_ENCODING_GSM,
    ENC_MP3 = SOX_ENCODING_MP3,
    ENC_VORBIS = SOX_ENCODING_VORBIS,
    ENC_AMR_WB = SOX_ENCODING_AMR_WB,
    ENC_AMR_NB = SOX_ENCODING_AMR_NB,
    ENC_CVSD = SOX_ENCODING_CVSD,
    ENC_LPC10= SOX_ENCODING_LPC10,
    ENC_OPUS = SOX_ENCODING_OPUS,
};

class SoxxSamples
{
    soxformat_t format;
    sox_signalinfo_t info;    
    std::vector<sox_ssample_t> buffer;

    SoxxSamples() {
        
    }
    ~SoxxSamples() {
        
    }

    const char* filename() { return format.filename; }
    double rate() const { return info.rate; }
    unsigned channels() const { return info.channels; }
    unsigned precision() const { return info.precision; }
    uint64_t length() const { return info.length; }
    double*  mult() { return info.mult; }

};

class SoxxReader
{
public:

    
    SoxxReader() {
        
    }
    ~SoxxReader() {
        
    }

    
};

void init_sox() {
    int r = sox_format_init();
}
void close_sox() {
    sox_format_close();
}