ffi = require('ffi')
ffi.cdef[[
int StreamCallback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

void*  get_stream();
void** get_stream_address();
void test();
struct addressptr addressof();
void RunLoop();
]]

addressof = ffi.load('addressof')