#pragma once

#include "Spectacle.hpp"


namespace DSP::Spectacle
{
    // 2x <-> 1x: TBW = 0.01
    static constexpr double C2x[12] = {
        0.036681502163648017,
        0.136547624631957715,
        0.274631759379454110,
        0.423138617436566666,
        0.561098697879194752,
        0.677540049974161618,
        0.769741833863226588,
        0.839889624849638028,
        0.892260818003878908,
        0.931541959963183896,
        0.962094548378083947,
        0.987816370732897076,
    };
    // 4x <-> 2x: TBW = 0.255
    static constexpr double C4x[4] = {
        0.041893991997656171,
        0.168903482439952013,
        0.390560772921165922,
        0.743895748268478152,
    };
    // 8x <-> 4x: TBW = 0.3775
    static constexpr double C8x[3] = {
        0.055748680811302048,
        0.243051195741530918,
        0.646699131192682297,
    };
    // 16x <-> 8x: TBW = 0.43875
    static constexpr double C16x[2] = {
        0.107172166664564611,
        0.530904350331903085,
    };
    // 32x <-> 16x: TBW = 0.469375
    static constexpr double C32x[2] = {
        0.105969237763476387,
        0.528620279623742473,
    };
    // 64x <-> 32x: TBW = 0.484687
    static constexpr double C64x[1] = {
        0.333526281707771211,
    };
    // 128x <-> 64x: TBW = 0.492344
    static constexpr double C128x[1] = {
        0.333381553051105561,
    };

    static constexpr uint32_t NC2x = sizeof(C2x) / sizeof(double);
    static constexpr uint32_t NC4x = sizeof(C4x) / sizeof(double);
    static constexpr uint32_t NC8x = sizeof(C8x) / sizeof(double);
    static constexpr uint32_t NC16x = sizeof(C16x) / sizeof(double);
    static constexpr uint32_t NC32x = sizeof(C32x) / sizeof(double);
    static constexpr uint32_t NC64x = sizeof(C64x) / sizeof(double);
    static constexpr uint32_t NC128x = sizeof(C128x) / sizeof(double);

    ///
    class BasicUpsampler {
    public:
        virtual ~BasicUpsampler() {}
        virtual void upsample(uint32_t count, const float *input, float **outputs) = 0;
        virtual void clear() = 0;
    };

    ///
    template <uint32_t Log2Factor> class Upsampler;

    ///
    class BasicDownsampler {
    public:
        virtual ~BasicDownsampler() {}
        virtual void downsample(uint32_t count, const float *input, float **outputs) = 0;
        virtual void clear() = 0;
    };

    ///
    template <uint32_t Log2Factor> class Downsampler;


    ///
    struct UpsamplerStage2x : public hiir::Upsampler2xFpu<NC2x> {
        UpsamplerStage2x() { set_coefs(C2x); }
    };

    ///
    template <>
    class Upsampler<1> final : public BasicUpsampler {
    public:
        enum {
            Factor = 2,
            Log2Factor = 1,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
        }

    private:
        UpsamplerStage2x _stage2x;
    };

    using Upsampler2x = Upsampler<1>;

    ///
    struct UpsamplerStage4x : public hiir::Upsampler2xFpu<NC4x> {
        UpsamplerStage4x() { set_coefs(C4x); }
    };

    ///
    template <>
    class Upsampler<2> final : public BasicUpsampler {
    public:
        enum {
            Factor = 4,
            Log2Factor = 2,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
        }

    private:
        UpsamplerStage2x _stage2x;
        UpsamplerStage4x _stage4x;
    };

    using Upsampler4x = Upsampler<2>;

    ///
    struct UpsamplerStage8x : public hiir::Upsampler2xFpu<NC8x> {
        UpsamplerStage8x() { set_coefs(C8x); }
    };

    ///
    template <>
    class Upsampler<3> final : public BasicUpsampler {
    public:
        enum {
            Factor = 8,
            Log2Factor = 3,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
            _stage8x.process_block(outputs[2], outputs[1], count * 4);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
        }

    private:
        UpsamplerStage2x _stage2x;
        UpsamplerStage4x _stage4x;
        UpsamplerStage8x _stage8x;
    };

    using Upsampler8x = Upsampler<3>;

    ///
    struct UpsamplerStage16x : public hiir::Upsampler2xFpu<NC16x> {
        UpsamplerStage16x() { set_coefs(C16x); }
    };

    ///
    template <>
    class Upsampler<4> final : public BasicUpsampler {
    public:
        enum {
            Factor = 16,
            Log2Factor = 4,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
            _stage8x.process_block(outputs[2], outputs[1], count * 4);
            _stage16x.process_block(outputs[3], outputs[2], count * 8);
        }

        void clear() override
        {
            _stage2x.clear_buffers();
            _stage4x.clear_buffers();
            _stage8x.clear_buffers();
            _stage16x.clear_buffers();
        }

    private:
        UpsamplerStage2x _stage2x;
        UpsamplerStage4x _stage4x;
        UpsamplerStage8x _stage8x;
        UpsamplerStage16x _stage16x;
    };

    using Upsampler16x = Upsampler<4>;

    ///
    struct UpsamplerStage32x : public hiir::Upsampler2xFpu<NC32x> {
        UpsamplerStage32x() { set_coefs(C32x); }
    };

    ///
    template <>
    class Upsampler<5> final : public BasicUpsampler {
    public:
        enum {
            Factor = 32,
            Log2Factor = 5,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
            _stage8x.process_block(outputs[2], outputs[1], count * 4);
            _stage16x.process_block(outputs[3], outputs[2], count * 8);
            _stage32x.process_block(outputs[4], outputs[3], count * 16);
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
        UpsamplerStage2x _stage2x;
        UpsamplerStage4x _stage4x;
        UpsamplerStage8x _stage8x;
        UpsamplerStage16x _stage16x;
        UpsamplerStage32x _stage32x;
    };

    using Upsampler32x = Upsampler<5>;

    ///
    struct UpsamplerStage64x : public hiir::Upsampler2xFpu<NC64x> {
        UpsamplerStage64x() { set_coefs(C64x); }
    };

    ///
    template <>
    class Upsampler<6> final : public BasicUpsampler {
    public:
        enum {
            Factor = 64,
            Log2Factor = 6,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
            _stage8x.process_block(outputs[2], outputs[1], count * 4);
            _stage16x.process_block(outputs[3], outputs[2], count * 8);
            _stage32x.process_block(outputs[4], outputs[3], count * 16);
            _stage64x.process_block(outputs[5], outputs[4], count * 32);
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
        UpsamplerStage2x _stage2x;
        UpsamplerStage4x _stage4x;
        UpsamplerStage8x _stage8x;
        UpsamplerStage16x _stage16x;
        UpsamplerStage32x _stage32x;
        UpsamplerStage64x _stage64x;
    };

    using Upsampler64x = Upsampler<6>;

    ///
    struct UpsamplerStage128x : public hiir::Upsampler2xFpu<NC128x> {
        UpsamplerStage128x() { set_coefs(C128x); }
    };

    ///
    template <>
    class Upsampler<7> final : public BasicUpsampler {
    public:
        enum {
            Factor = 128,
            Log2Factor = 7,
        };

        void upsample(uint32_t count, const float *input, float **outputs) override
        {
            _stage2x.process_block(outputs[0], input, count);
            _stage4x.process_block(outputs[1], outputs[0], count * 2);
            _stage8x.process_block(outputs[2], outputs[1], count * 4);
            _stage16x.process_block(outputs[3], outputs[2], count * 8);
            _stage32x.process_block(outputs[4], outputs[3], count * 16);
            _stage64x.process_block(outputs[5], outputs[4], count * 32);
            _stage128x.process_block(outputs[6], outputs[5], count * 64);
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
        UpsamplerStage2x _stage2x;
        UpsamplerStage4x _stage4x;
        UpsamplerStage8x _stage8x;
        UpsamplerStage16x _stage16x;
        UpsamplerStage32x _stage32x;
        UpsamplerStage64x _stage64x;
        UpsamplerStage128x _stage128x;
    };

    using Upsampler128x = Upsampler<7>;
}