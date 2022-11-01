#pragma once 

namespace DSP {
        ////////////////////////////////////////////////////////////////////////////////
    // Sound File I/O
    // KFR and libnyquist
    ////////////////////////////////////////////////////////////////////////////////

    using seek_origin = kfr::seek_origin;
    using audio_format = kfr::audio_format_and_length;

    
    template<typename T>
    struct WavReader {
    private:
        kfr::audio_reader_wav<T> *reader;
                
    public:
        WavReader(const char * filename) {
                    
            reader = new kfr::audio_reader_wav<T>(kfr::open_file_for_reading(filename));    
            assert(reader != NULL);
        }
        ~WavReader() {
            if(reader) {                
                delete reader;                
            }
        }
        size_t read(kfr::univector<T> & vec) {
            return reader->read(vec.data(),vec.size());
        }

        size_t tell() const {
            return reader->tell();
        }

        
        bool seek(size_t offset, seek_origin origin ) {
            return reader->seek(offset,origin);
        }        
        bool seek_begin(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::begin);
        }
        bool seek_current(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::current);
        }
        bool seek_end(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::end);
        }
        
        const audio_format& format() {
            return reader->format();
        }
        size_t size() { 
            const audio_format& fmt = format();
            return fmt.length;
        }
    };

    template<typename T>
    kfr::univector<T> load_wav(const char * filename) {
        WavReader<T> r(filename);        
        kfr::univector<T> v(r.size());
        r.read(v);
        return v;
    }

    template<typename T>
    struct WavWriter {
        kfr::audio_writer_wav<T> * writer;
        
        
        WavWriter(const char * filename, const audio_format & format) {            
            writer = new kfr::audio_writer_wav<T>(kfr::open_file_for_writing(filename),format);    
            assert(writer != NULL);
        }
        ~WavWriter() {
            if(writer) {                                
                delete writer;
            }
        }
        void write(kfr::univector<T> & vec) {
            writer->write(vec.data(),vec.size());
        }
        const audio_format& format() {
            return writer->format();
        }
    };

    template<typename T>
    void write_wav(kfr::univector<T> & v, const char * filename, size_t channels, int sample_type, double sample_rate, bool use_w64=false) {
        WavWriter<T> w(filename,kfr::audio_format{channels,static_cast<kfr::audio_sample_type>(sample_type),sample_rate,use_w64});
        w.write(v);        
    }

    template<typename T>
    struct MP3Reader {
        kfr::audio_reader_mp3<T> * reader;
        
        MP3Reader(const char * filename) {            
            reader = new kfr::audio_reader_mp3<T>(kfr::open_file_for_reading(filename));    
            assert(reader != NULL);
        }
        ~MP3Reader() {
            if(reader != NULL) {                
                delete reader;
            }
        }
        size_t read(kfr::univector<T> & vec) {
            return reader->read(vec.data(),vec.size());
        }

        size_t tell() const {
            return reader->tell();
        }

        bool seek(size_t offset, seek_origin origin ) {
            return reader->seek(offset,origin);
        }
        bool seek_begin(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::begin);
        }
        bool seek_current(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::current);
        }
        bool seek_end(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::end);
        }
        
        const audio_format& format() {
            return reader->format();
        }

        size_t size()  { 
            const audio_format& fmt = format();
            return fmt.length;
        }

    };

    template<typename T>
    kfr::univector<T> load_mp3(const char * filename) {
        MP3Reader<T> r(filename);
        kfr::univector<T> v(r.size());
        r.read(v);
        return v;
    }
    template<typename T>
    struct FlacReader {
        kfr::audio_reader_flac<T> *reader;
                
        FlacReader(const char * filename) {                    
            reader = new kfr::audio_reader_flac<T>(kfr::open_file_for_reading(filename));    
            assert(reader != NULL);
        }
        ~FlacReader() {
            if(reader != NULL) {
                delete reader;
            }
        }
        size_t read(kfr::univector<T> & vec) {
            return reader->read(vec.data(),vec.size());
        }

        size_t tell() const {
            return reader->tell();
        }

        bool seek(size_t offset, seek_origin origin ) {
            return reader->seek(offset,origin);
        }
        bool seek_begin(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::begin);
        }
        bool seek_current(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::current);
        }
        bool seek_end(size_t offset)
        {
            return reader->seek(offset,kfr::seek_origin::end);
        }
        
        const audio_format& format() {
            return reader->format();
        }
        size_t size() { 
            const audio_format& fmt = format();
            return fmt.length;
        }
    };

    template<typename T>
    kfr::univector<T> load_flac(const char * filename) {
        FlacReader<T> r(filename);
        kfr::univector<T> v(r.size());
        r.read(v);
        return v;
    }
}