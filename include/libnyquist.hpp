#pragma once 

#include <Common.h>
#include <Decoders.h>
#include <Encoders.h>

namespace SoundWave {
struct AudioData 
{
    nqr::AudioData data;

    AudioData(const AudioData & d) { data = (nqr::AudioData&)d; }

    size_t channels() const { return data.channelCount; }
    int sample_rate() const { return data.sampleRate; }
    double length_seconds() const { return data.lengthSeconds; }
    size_t frame_size() const { return data.frameSize; }
    std::vector<float>& get_samples() { return data.samples; }
    nqr::PCMFormat format() const { return data.sourceFormat; }

    float& operator[](size_t i) { return data.samples[i]; }
    float& operator()(size_t i, size_t c) { return data.samples[i*data.channelCount + c]; }
    
    void __setitem(size_t i, float value) { data.samples[i] = value; }
    float __getitem(size_t i) { return data.samples[i]; }

    float* get_channel(size_t c) {
        size_t x = 0;
        float * r = (float*)calloc(data.samples.size()/data.channelCount,sizeof(float));
        for(size_t i = c; i < data.samples.size(); i += data.channelCount)
            r[x++] = data.samples[i];
        return r;
    }
    void set_channel(float * data_ptr, size_t c ) {
        size_t x = 0;
        for(size_t i = c; i < data.samples.size(); i += data.channelCount)
            data.samples[i] = data_ptr[x++];
    }
    float* ptr() { return data.samples.data(); }
    
};

struct EncoderParams 
{
    nqr::EncoderParams params;

    EncoderParams(int channels, nqr::PCMFormat targetFormat, nqr::DitherType dither ) {
        params.channelCount = channels;
        params.targetFormat = targetFormat;
        params.dither       = dither;
    }

    int& channels()  { return params.channelCount; }
    nqr::PCMFormat& format() { return params.targetFormat; }
    nqr::DitherType& dither() { return params.dither; }
};

template<typename T>
void DeinterleaveStereo(T * c1, T * c2, T const * src, size_t count) {
    DeinterleaveStereo(c1,c2,src,count);
}
template<typename T>
void StereoToMono(const T * src, T * dst, size_t N) {
    StereoToMono(src,dst,N);
}
template<typename T>
void MonoToStereo(const T * src, T * dst, size_t N) {
    MonoToStereo(src,dst,N);
}

inline void WavDecoder(const std::string & filename, AudioData & data) {
    nqr::WavDecoder w;
    w.LoadFromPath(&data.data,filename);    
}
inline void WavPackDecoder(const std::string & filename, AudioData & data) {
    nqr::WavPackDecoder w;
    w.LoadFromPath(&data.data,filename);    
}
inline void VorbisDecoder(const std::string & filename, AudioData & data) {
    nqr::VorbisDecoder w;
    w.LoadFromPath(&data.data,filename);    
}
inline void OpusDecoder(const std::string & filename, AudioData & data) {
    nqr::OpusDecoder w;
    w.LoadFromPath(&data.data,filename);    
}
inline void MusepackDecoder(const std::string & filename, AudioData & data) {
    nqr::MusepackDecoder w;
    w.LoadFromPath(&data.data,filename);    
}
inline void Mp3Decoder(const std::string & filename, AudioData & data) {
    nqr::Mp3Decoder w;
    w.LoadFromPath(&data.data,filename);    
}
inline void FlacDecoder(const std::string & filename, AudioData & data) {
    nqr::FlacDecoder w;
    w.LoadFromPath(&data.data,filename);    
}

inline int WavEncoder(const std::string & filename, EncoderParams & p, const AudioData & d) {
    return nqr::encode_wav_to_disk(p.params, &d.data, filename);
}
inline int OpusEncoder(const std::string & filename, EncoderParams & p, const AudioData & d) {
    return nqr::encode_opus_to_disk(p.params,&d.data,filename);
}
};





