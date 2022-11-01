#pragma once

#include "Spectacle.hpp"


namespace DSP::Spectacle
{

    ///
    struct DownsamplerStage2x : public hiir::Downsampler2xFpu<NC2x> {
        DownsamplerStage2x() { set_coefs(C2x); }
    };

    template <>
    class Downsampler<1> final : public BasicDownsampler {
    public:
        enum {
            Factor = 2,
            Log2Factor = 1,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
    };

    using Downsampler2x = Downsampler<1>;

    ///
    struct DownsamplerStage4x : public hiir::Downsampler2xFpu<NC4x> {
        DownsamplerStage4x() { set_coefs(C4x); }
    };

    template <>
    class Downsampler<2> final : public BasicDownsampler {
    public:
        enum {
            Factor = 4,
            Log2Factor = 2,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count * 2);
            _stage4x.process_block(outputs[1], outputs[0], count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
        DownsamplerStage4x _stage4x;
    };

    using Downsampler4x = Downsampler<2>;

    ///
    struct DownsamplerStage8x : public hiir::Downsampler2xFpu<NC8x> {
        DownsamplerStage8x() { set_coefs(C8x); }
    };

    template <>
    class Downsampler<3> final : public BasicDownsampler {
    public:
        enum {
            Factor = 8,
            Log2Factor = 3,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count * 4);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
            _stage8x.process_block(outputs[2], outputs[1], count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
        DownsamplerStage4x _stage4x;
        DownsamplerStage8x _stage8x;
    };

    using Downsampler8x = Downsampler<3>;

    ///
    struct DownsamplerStage16x : public hiir::Downsampler2xFpu<NC16x> {
        DownsamplerStage16x() { set_coefs(C16x); }
    };

    template <>
    class Downsampler<4> final : public BasicDownsampler {
    public:
        enum {
            Factor = 16,
            Log2Factor = 4,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count * 8);
            _stage4x.process_block(outputs[1], outputs[0], count * 4);
            _stage8x.process_block(outputs[2], outputs[1], count * 2);
            _stage16x.process_block(outputs[3], outputs[2], count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
            _stage16x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
        DownsamplerStage4x _stage4x;
        DownsamplerStage8x _stage8x;
        DownsamplerStage16x _stage16x;
    };

    using Downsampler16x = Downsampler<4>;

    ///
    struct DownsamplerStage32x : public hiir::Downsampler2xFpu<NC32x> {
        DownsamplerStage32x() { set_coefs(C32x); }
    };

    template <>
    class Downsampler<5> final : public BasicDownsampler {
    public:
        enum {
            Factor = 32,
            Log2Factor = 5,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count * 16);
            _stage4x.process_block(outputs[1], outputs[0], count * 8);
            _stage8x.process_block(outputs[2], outputs[1], count * 4);
            _stage16x.process_block(outputs[3], outputs[2], count * 2);
            _stage32x.process_block(outputs[4], outputs[3], count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
            _stage16x.clear_buffers();
            _stage32x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
        DownsamplerStage4x _stage4x;
        DownsamplerStage8x _stage8x;
        DownsamplerStage16x _stage16x;
        DownsamplerStage32x _stage32x;
    };

    using Downsampler32x = Downsampler<5>;

    ///
    struct DownsamplerStage64x : public hiir::Downsampler2xFpu<NC64x> {
        DownsamplerStage64x() { set_coefs(C64x); }
    };

    template <>
    class Downsampler<6> final : public BasicDownsampler {
    public:
        enum {
            Factor = 64,
            Log2Factor = 6,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count * 32);
            _stage4x.process_block(outputs[1], outputs[0], count * 16);
            _stage8x.process_block(outputs[2], outputs[1], count * 8);
            _stage16x.process_block(outputs[3], outputs[2], count * 4);
            _stage32x.process_block(outputs[4], outputs[3], count * 2);
            _stage64x.process_block(outputs[5], outputs[4], count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
            _stage16x.clear_buffers();
            _stage32x.clear_buffers();
            _stage64x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
        DownsamplerStage4x _stage4x;
        DownsamplerStage8x _stage8x;
        DownsamplerStage16x _stage16x;
        DownsamplerStage32x _stage32x;
        DownsamplerStage64x _stage64x;
    };

    using Downsampler64x = Downsampler<6>;

    ///
    struct DownsamplerStage128x : public hiir::Downsampler2xFpu<NC128x> {
        DownsamplerStage128x() { set_coefs(C128x); }
    };

    template <>
    class Downsampler<7> final : public BasicDownsampler {
    public:
        enum {
            Factor = 128,
            Log2Factor = 7,
        };

        void downsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count * 64);
            _stage4x.process_block(outputs[1], outputs[0], count * 32);
            _stage8x.process_block(outputs[2], outputs[1], count * 16);
            _stage16x.process_block(outputs[3], outputs[2], count * 8);
            _stage32x.process_block(outputs[4], outputs[3], count * 4);
            _stage64x.process_block(outputs[5], outputs[4], count * 2);
            _stage128x.process_block(outputs[6], outputs[5], count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
            _stage16x.clear_buffers();
            _stage32x.clear_buffers();
            _stage64x.clear_buffers();
            _stage128x.clear_buffers();
        }

    private:
        DownsamplerStage2x _stage2x;
        DownsamplerStage4x _stage4x;
        DownsamplerStage8x _stage8x;
        DownsamplerStage16x _stage16x;
        DownsamplerStage32x _stage32x;
        DownsamplerStage64x _stage64x;
        DownsamplerStage128x _stage128x;
    };

    using Downsampler128x = Downsampler<7>;

}