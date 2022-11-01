


%module kfr 
%{
#include "KFR.h"

using namespace kfr;
using namespace DSP;
%}

%include "std_vector.i"
%include "std_string.i"

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;


%include "KFR.h"
using namespace DSP;


typedef int8_t   i8;
typedef uint8_t   u8;
typedef int16_t  i16;
typedef uint16_t  u16;
typedef int32_t  i32;
typedef uint32_t  u32;
typedef int64_t  i64;
typedef float  f32;
typedef double  f64;


%template(float_vector) std::vector<f32>;
%template(double_vector) std::vector<f64>;
%template(int8_vector) std::vector<i8>;
%template(uint8_vector) std::vector<u8>;
%template(int16_vector) std::vector<i16>;
%template(uint16_vector) std::vector<u16>;
%template(int32_vector) std::vector<i32>;
%template(uint32_vector) std::vector<u32>;
%template(int64_vector) std::vector<i64>;

%template(FloatScalar) Scalar<f32>;
%template(DoubleScalar) Scalar<f64>;
%template(Int8Scalar) Scalar<i8>;
%template(UInt8Scalar) Scalar<u8>;
%template(Int16Scalar) Scalar<i16>;
%template(UInt16Scalar) Scalar<u16>;
%template(Int32Scalar) Scalar<i32>;
%template(UInt32Scalar) Scalar<u32>;
%template(Int64Scalar) Scalar<i64>;
%template(UInt64Scalar) Scalar<u64>;

%template(FloatComplex) Complex<float>;
%template(DoubleComplex) Complex<double>;

%template(FloatVector) UniVector<f32>;
%template(DoubleVector) UniVector<f64>;
%template(ComplexVector) UniVector<kfr::complex<f32>>;
%template(DoubleComplexVector) UniVector<kfr::complex<f64>>;
%template(Int8Vector) UniVector<i8>;
%template(UInt8Vector) UniVector<u8>;
%template(Int16Vector) UniVector<i16>;
%template(UInt16Vector) UniVector<u16>;
%template(Int32Vector) UniVector<i32>;
%template(UInt32Vector) UniVector<u32>;
%template(Int64Vector) UniVector<i64>;
%template(UInt64Vector) UniVector<u64>;

%template(FloatVector2D) UniVector2D<f32>;
%template(DoubleVector2D) UniVector2D<f64>;

%template(FloatBiquad)  Biquad<f32>;
%template(DoubleBiquad)  Biquad<f64>;
%template(FloatBiQuadParams)  BiQuadParams<f32>;
%template(DoubleBiQuadParams)  BiQuadParams<f64>;
%template(biquadf)  biquad<f32>;
%template(biquadd)  biquad<f64>;
%template(notchf)  notch<f32>;
%template(notchd)  notch<f64>;
%template(lowpassf)  lowpass<f32>;
%template(lowpassd)  lowpass<f64>;
%template(highpassf)  highpass<f32>;
%template(highpassd)  highpass<f64>;
%template(peakf)  peak<f32>;
%template(peakd)  peak<f64>;
%template(lowshelff)  lowshelf<f32>;
%template(lowshelfd)  lowshelf<f64>;
%template(highshelff)  highshelf<f32>;
%template(highshelfd)  highshelf<f64>;
%template(bandpassf)  bandpass<f32>;
%template(bandpassd)  bandpass<f64>;
%template(FloatNotchFilter) NotchFilter<f32>;
%template(DoubleNotchFilter) NotchFilter<f64>;
%template(FloatLowPassFilter) LowPassFilter<f32>;
%template(DoubleLowPassFilter) LowPassFilter<f64>;
%template(FloatHighPassFilter) HighPassFilter<f32>;
%template(DoubleHighPassFilter) HighPassFilter<f64>;
%template(FloatBandPassFilter) BandPassFilter<f32>;
%template(DoubleBandPassFilter) BandPassFilter<f64>;
%template(FloatPeakFilter) PeakFilter<f32>;
%template(DoublePeakFilter) PeakFilter<f64>;
%template(FloatLowShelfFilter) LowShelfFilter<f32>;
%template(DoubleLowShelfFilter) LowShelfFilter<f64>;
%template(FloatHighShelfFilter) HighShelfFilter<f32>;
%template(DoubleHighShelfFilter) HighShelfFilter<f64>;

%template(normalize_frequencyf) normalize_frequency<f32>;
%template(normalize_frequencyd) normalize_frequency<f64>;


%template(linspacef) linspace<f32>;
%template(padf) pad<f32>;
%template(slicef) slice<f32>;
%template(truncatef) truncate<f32>;
%template(reversef) reverse<f32>;

%template(linspaced) linspace<f64>;
%template(padd) pad<f64>;
%template(sliced) slice<f64>;
%template(truncated) truncate<f64>;
%template(reversed) reverse<f64>;

%template(ringbuf_readf) ringbuf_read<f32>;
%template(ringbuf_writef) ringbuf_write<f32>;

%template(ringbuf_readd) ringbuf_read<f64>;
%template(ringbuf_writed) ringbuf_write<f64>;

%template(absf) abs<f32>;
%template(addf) add<f32>;
%template(absmaxf) absmax<f32>;
%template(absminf) absmin<f32>;
%template(clampf) clamp<f32>;
%template(cubef) cube<f32>;
%template(divf) div<f32>;
%template(fmaddf) fmadd<f32>;
%template(fmsubf) fmsub<f32>;
%template(maxf) max<f32>;
%template(minf) min<f32>;
%template(mixf) mix<f32>;
%template(mixsf) mixs<f32>;
%template(mulf) mul<f32>;
%template(negf) neg<f32>;
%template(sqrf) sqr<f32>;
%template(sqrtf) sqrt<f32>;
%template(expf) exp<f32>;
%template(exp10f) exp10<f32>;
%template(exp2f) exp2<f32>;
%template(exp_fmaddf) exp_fmadd<f32>;
%template(logf) log<f32>;
%template(log10f) log10<f32>;
%template(log2f) log2<f32>;
%template(log_fmaddf) log_fmadd<f32>;
%template(logbf) logb<f32>;
%template(logmf) logm<f32>;
%template(lognf) logn<f32>;
%template(powf) pow<f32>;
%template(rootf) root<f32>;
%template(floorf) floor<f32>;
%template(acosf) acos<f32>;
%template(asinf) asin<f32>;
%template(atanf) atan<f32>;
%template(atan2f) atan2<f32>;
%template(cosf) cos<f32>;
%template(sinf) sin<f32>;
%template(tanf) tan<f32>;
%template(coshf) cosh<f32>;
%template(cothf) coth<f32>;
%template(sinhf) sinh<f32>;
%template(tanhf) tanh<f32>;
%template(atan2degf) atan2deg<f32>;
%template(cosdegf) cosdeg<f32>;
%template(sindegf) sindeg<f32>;
%template(sincf) sinc<f32>;
%template(gammaf) gamma<f32>;
%template(absmaxof) absmaxof<f32>;
%template(dotf) dot<f32>;
%template(maxoff) maxof<f32>;
%template(minoff) minof<f32>;
%template(meanf) mean<f32>;
%template(prdocutf) product<f32>;
%template(rmsf) rms<f32>;
%template(sumf) sum<f32>;
%template(sumsqrf) sumsqr<f32>;
%template(ipowf) ipow<f32>;
%template(cos2xf) kcos2x<f32>;
%template(sin2xf) ksin2x<f32>;
%template(cos3xf) kcos3x<f32>;
%template(sin3xf) ksin3x<f32>;
%template(reciprocalf) kreciprocal<f32>;

%template(acoshf) acosh<f32>;
%template(asinhf) asinh<f32>;
%template(atanhf) atanh<f32>;
%template(cbrtf) cbrt<f32>;
%template(ceilf) cbrt<f32>;
%template(copysignf) copysign<f32>;
%template(erff) erf<f32>;
%template(erfcf) erfc<f32>;
%template(expm1f) expm1<f32>;
%template(fdimf) fdim<f32>;
%template(fmaf) fma<f32>;
%template(fmaxf) fmax<f32>;
%template(fminf) fmin<f32>;
%template(fmodf) fmod<f32>;
%template(fpclassifyf) fpclassify<f32>;
%template(hypotf) hypot<f32>;
%template(ilogbf) ilogb<f32>;
%template(isfinitef) isfinite<f32>;
%template(isgreaterf) isgreater<f32>;
%template(isgreaterequalf) isgreaterequal<f32>;
%template(isinff) isinf<f32>;
%template(islessf) isless<f32>;
%template(islessequalf) islessequal<f32>;
%template(isnanf) isnan<f32>;
%template(isnormalf) isnormal<f32>;
%template(isunorderedf) isunordered<f32>;
%template(ldexpf) ldexp<f32>;
%template(lgammaf) lgamma<f32>;
%template(llrintf) llrint<f32>;
%template(llroundf) llround<f32>;
%template(log1pf) log1p<f32>;
%template(lrintf) lrint<f32>;
%template(lroundf) lround<f32>;
%template(nanf) nan<f32>;
%template(nanff) nanf<f32>;
%template(nanlf) nanl<f32>;
%template(nearbyintf) nearbyint<f32>;
%template(nextafterf) nextafter<f32>;
%template(nexttowardf) nexttoward<f32>;
%template(remainderf) remainder<f32>;
%template(rintf) rint<f32>;
%template(roundf) round<f32>;
%template(scalblnf) scalbln<f32>;
%template(scalbnf) scalbn<f32>;
%template(squaref) square<f32>;
%template(tgammaf) tgamma<f32>;
%template(truncf) trunc<f32>;

%template(linspaced) linspace<f64>;
%template(padd) pad<f64>;
%template(sliced) slice<f64>;
%template(truncated) truncate<f64>;
%template(reversed) reverse<f64>;

%template(absd) abs<f64>;
%template(addd) add<f64>;
%template(absmaxd) absmax<f64>;
%template(absmind) absmin<f64>;
%template(clampd) clamp<f64>;
%template(cubed) cube<f64>;
%template(divd) div<f64>;
%template(fmaddd) fmadd<f64>;
%template(fmsubd) fmsub<f64>;
%template(maxd) max<f64>;
%template(mind) min<f64>;
%template(mixd) mix<f64>;
%template(mixsd) mixs<f64>;
%template(muld) mul<f64>;
%template(negd) neg<f64>;
%template(sqrd) sqr<f64>;
%template(sqrtd) sqrt<f64>;
%template(expd) exp<f64>;
%template(exp10d) exp10<f64>;
%template(exp2d) exp2<f64>;
%template(exp_fmaddd) exp_fmadd<f64>;
%template(logd) log<f64>;
%template(log10d) log10<f64>;
%template(log2d) log2<f64>;
%template(log_fmaddd) log_fmadd<f64>;
%template(logbd) logb<f64>;
%template(logmd) logm<f64>;
%template(lognd) logn<f64>;
%template(powd) pow<f64>;
%template(rootd) root<f64>;
%template(floord) floor<f64>;
%template(acosd) acos<f64>;
%template(asind) asin<f64>;
%template(atand) atan<f64>;
%template(atan2d) atan2<f64>;
%template(cosd) cos<f64>;
%template(sind) sin<f64>;
%template(tand) tan<f64>;
%template(coshd) cosh<f64>;
%template(cothd) coth<f64>;
%template(sinhd) sinh<f64>;
%template(tanhd) tanh<f64>;
%template(atan2degd) atan2deg<f64>;
%template(cosdegd) cosdeg<f64>;
%template(sindegd) sindeg<f64>;
%template(sincd) sinc<f64>;
%template(gammad) gamma<f64>;
%template(absmaxod) absmaxof<f64>;
%template(dotd) dot<f64>;
%template(maxofd) maxof<f64>;
%template(minofd) minof<f64>;
%template(meand) mean<f64>;
%template(prdocutd) product<f64>;
%template(rmsd) rms<f64>;
%template(sumd) sum<f64>;
%template(sumsqrd) sumsqr<f64>;
%template(ipowd) ipow<f64>;
%template(cos2xd) kcos2x<f64>;
%template(sin2xd) ksin2x<f64>;
%template(cos3xd) kcos3x<f64>;
%template(sin3xd) ksin3x<f64>;
%template(reciprocald) kreciprocal<f64>;

%template(acoshd) acosh<f64>;
%template(asinhd) asinh<f64>;
%template(atanhd) atanh<f64>;
%template(cbrtd) cbrt<f64>;
%template(ceild) cbrt<f64>;
%template(copysignd) copysign<f64>;
%template(erfd) erf<f64>;
%template(erfcd) erfc<f64>;
%template(expm1d) expm1<f64>;
%template(fdimd) fdim<f64>;
%template(fmad) fma<f64>;
%template(fmaxd) fmax<f64>;
%template(fmind) fmin<f64>;
%template(fmodd) fmod<f64>;
%template(fpclassifyd) fpclassify<f64>;
%template(hypotd) hypot<f64>;
%template(ilogbd) ilogb<f64>;
%template(isfinited) isfinite<f64>;
%template(isgreaterd) isgreater<f64>;
%template(isgreaterequald) isgreaterequal<f64>;
%template(isinfd) isinf<f64>;
%template(islessd) isless<f64>;
%template(islessequald) islessequal<f64>;
%template(isnand) isnan<f64>;
%template(isnormald) isnormal<f64>;
%template(isunorderedd) isunordered<f64>;
%template(ldexpd) ldexp<f64>;
%template(lgammad) lgamma<f64>;
%template(llrintd) llrint<f64>;
%template(llroundd) llround<f64>;
%template(log1pd) log1p<f64>;
%template(lrintd) lrint<f64>;
%template(lroundd) lround<f64>;
%template(nand) nan<f64>;
%template(nanfd) nanf<f64>;
%template(nanld) nanl<f64>;
%template(nearbyintd) nearbyint<f64>;
%template(nextafterd) nextafter<f64>;
%template(nexttowardd) nexttoward<f64>;
%template(remainderd) remainder<f64>;
%template(rintd) rint<f64>;
%template(roundd) round<f64>;
%template(scalblnd) scalbln<f64>;
%template(scalbnd) scalbn<f64>;
%template(squared) square<f64>;
%template(tgammad) tgamma<f64>;
%template(truncd) trunc<f64>;


%template(make_complexf) make_complex<f32>;
%template(make_complexd) make_complex<f64>;

%template(make_univectorf) make_univec<f32>;
%template(make_univectord) make_univec<f64>;
%template(make_univectori8) make_univec<i8>;
%template(make_univectoru8) make_univec<u8>;
%template(make_univectori16) make_univec<i16>;
%template(make_univectoru16) make_univec<u16>;
%template(make_univectori32) make_univec<i32>;
%template(make_univectoru32) make_univec<u32>;

%template(window_hannf) make_window_hann<f32>;
%template(window_hammingf) make_window_hamming<f32>;
%template(window_blackmanf) make_window_blackman<f32>;
%template(window_blackman_harrisf) make_window_blackman_harris<f32>;
%template(window_gaussianf) make_window_gaussian<f32>;
%template(window_triangularf) make_window_triangular<f32>;
%template(window_bartlettf) make_window_bartlett<f32>;
%template(window_cosinef) make_window_cosine<f32>;
%template(window_bartlett_hannf) make_window_bartlett_hann<f32>;
%template(window_bohmanf) make_window_bohman<f32>;
%template(window_lanczosf) make_window_lanczos<f32>;
%template(window_flattopf) make_window_flattop<f32>;
%template(window_kaiserf) make_window_kaiser<f32>;

%template(window_hannd) make_window_hann<f64>;
%template(window_hammingd) make_window_hamming<f64>;
%template(window_blackmand) make_window_blackman<f64>;
%template(window_blackman_harrisd) make_window_blackman_harris<f64>;
%template(window_gaussiand) make_window_gaussian<f64>;
%template(window_triangulard) make_window_triangular<f64>;
%template(window_bartlettd) make_window_bartlett<f64>;
%template(window_cosined) make_window_cosine<f64>;
%template(window_bartlett_hannd) make_window_bartlett_hann<f64>;
%template(window_bohmand) make_window_bohman<f64>;
%template(window_lanczosd) make_window_lanczos<f64>;
%template(window_dlattopd) make_window_flattop<f64>;
%template(window_kaiserd) make_window_kaiser<f64>;

%template(window_hann_ptrf) make_window_hann_ptr<f32>;
%template(window_hamming_ptrf) make_window_hamming_ptr<f32>;
%template(window_blackman_ptrf) make_window_blackman_ptr<f32>;
%template(window_blackman_harris_ptrf) make_window_blackman_harris_ptr<f32>;
%template(window_gaussian_ptrf) make_window_gaussian_ptr<f32>;
%template(window_triangular_ptrf) make_window_triangular_ptr<f32>;
%template(window_bartlett_ptrf) make_window_bartlett_ptr<f32>;
%template(window_cosine_ptrf) make_window_cosine_ptr<f32>;
%template(window_bartlett_hann_ptrf) make_window_bartlett_hann_ptr<f32>;
%template(window_bohman_ptrf) make_window_bohman_ptr<f32>;
%template(window_lanczos_ptrf) make_window_lanczos_ptr<f32>;
%template(window_flattop_ptrf) make_window_flattop_ptr<f32>;
%template(window_kaiser_ptrf) make_window_kaiser_ptr<f32>;

%template(window_hann_ptrd) make_window_hann_ptr<f64>;
%template(window_hamming_ptrd) make_window_hamming_ptr<f64>;
%template(window_blackman_ptrd) make_window_blackman_ptr<f64>;
%template(window_blackman_harris_ptrd) make_window_blackman_harris_ptr<f64>;
%template(window_gaussian_ptrd) make_window_gaussian_ptr<f64>;
%template(window_triangular_ptrd) make_window_triangular_ptr<f64>;
%template(window_bartlett_ptrd) make_window_bartlett_ptr<f64>;
%template(window_cosine_ptrd) make_window_cosine_ptr<f64>;
%template(window_bartlett_hann_ptrd) make_window_bartlett_hann_ptr<f64>;
%template(window_bohman_ptrd) make_window_bohman_ptr<f64>;
%template(window_lanczos_ptrd) make_window_lanczos_ptr<f64>;
%template(window_dlattop_ptrd) make_window_flattop_ptr<f64>;
%template(window_kaiser_ptrd) make_window_kaiser_ptr<f64>;

%template(slicef) slice<f32>;
%template(truncatef) truncate<f32>;
%template(reversef) reverse<f32>;
%template(ringbuf_readf) ringbuf_read<f32>;
%template(ringbuf_writef) ringbuf_write<f32>;

%template(sliced) slice<f64>;
%template(truncated) truncate<f64>;
%template(reversed) reverse<f64>;
%template(ringbuf_readd) ringbuf_read<f64>;
%template(ringbuf_writed) ringbuf_write<f64>;

%template(dftf) run_dft<f32>;
%template(realdftf) run_realdft<f32>;
%template(idftf) run_idft<f32>;
%template(irealdftf) run_irealdft<f32>;

%template(dftd) run_dft<f64>;
%template(realdftd) run_realdft<f64>;
%template(idftd) run_idft<f64>;
%template(irealdftd) run_irealdft<f64>;

%template(roli32) rol<i32>;
%template(ro4i32) ror<i32>;
%template(shli32) shl<i32>;
%template(shri32) shr<i32>;

%template(remi32)    rem<i32>;
%template(sataddi32) satadd<i32>;
%template(satsubi32) satsub<i32>;

%template(bitwiseandi32) bitwiseand<i32>;
%template(bitwiseori32) bitwiseor<i32>;
%template(bitwisexori32) bitwisexor<i32>;
%template(bitwiseandnoti32) bitwiseandnot<i32>;
%template(bitwisenoti32) bitwisenot<i32>;

%template(roli64) rol<i64>;
%template(ro4i64) ror<i64>;
%template(shli64) shl<i64>;
%template(shri64) shr<i64>;

%template(remi64)    rem<i64>;
%template(sataddi64) satadd<i64>;
%template(satsubi64) satsub<i64>;

%template(bitwiseandi64) bitwiseand<i64>;
%template(bitwiseori64) bitwiseor<i64>;
%template(bitwisexori64) bitwisexor<i64>;
%template(bitwiseandnoti64) bitwiseandnot<i64>;
%template(bitwisenoti64) bitwisenot<i64>;

%template(energy_to_loudnessf) energy_to_loudness<f32>;
%template(loudness_to_energyf) loudness_to_energy<f32>;

%template(energy_to_loudnessd) energy_to_loudness<f64>;
%template(loudness_to_energyd) loudness_to_energy<f64>;

%template(dcremovef) dcremove<f32>;
%template(dcremoved) dcremove<f64>;

%template(FloatWavReader) WavReader<f32>;
%template(DoubleWavReader) WavReader<f64>;

%template(FloatWavWriter) WavWriter<f32>;
%template(DoubleWavWriter) WavWriter<f64>;

%template(FloatMp3Reader) MP3Reader<f32>;
%template(DoubleMp3Reader) MP3Reader<f64>;

%template(FloatFlacReader) FlacReader<f32>;
%template(DoubleFlacReader) FlacReader<f64>;

%template (randomf) random<f32>;
%template (plot_savef) plot_save<f32>;
%template (plot_showf) plot_show<f32>;

%template (sinewavef) sinewave<f32>;
%template (squarewavef) squarewave<f32>;
%template (trianglewavef) trianglewave<f32>;
%template (sawtoothwavef) sawtoothwave<f32>;

%template (generate_sinef) generate_sin<f32>;
%template (generate_linearf) generate_linear<f32>;
%template (generate_expf) generate_exp<f32>;
%template (generate_exp2f) generate_exp2<f32>;
%template (generate_cossinf) generate_cossin<f32>;

%template (randomd) random<f64>;
%template (plot_saved) plot_save<f64>;
%template (plot_showd) plot_show<f64>;

%template (sinewaved) sinewave<f64>;
%template (squarewaved) squarewave<f64>;
%template (trianglewaved) trianglewave<f64>;
%template (sawtoothwaved) sawtoothwave<f64>;

%template (generate_sined) generate_sin<f64>;
%template (generate_lineard) generate_linear<f64>;
%template (generate_expd) generate_exp<f64>;
%template (generate_exp2d) generate_exp2<f64>;
%template (generate_cossind) generate_cossin<f64>;

%template (autocorrelatef) autocorrelate<float>;
%template (correlatef) correlate<float>;
%template (FloatDCTPlan) DCTPlan<float>;
%template (FloatDFTPlan) DFTPlan<float>;
%template (FloatDFTRealPlan) DFTRealPlan<float>;
%template (resamplef) resample<float>;
%template (convert_samplef) convert_sample<float>;
%template (amp_to_dBf) amp_to_dB<float>;

%template (FloatFIRFilter) FIRFilter<float>;
%template (FloatFIRBandpassFilter) FIRBandpassFilter<float>;
%template (FloatFIRLowpassFilter) FIRLowpassFilter<float>;
%template (FloatFIRHighpassFilter) FIRHighpassFilter<float>;
%template (FloatFIRBandstopFilter) FIRBandstopFilter<float>;

%template (fir_lowpassf) fir_lowpass<float>;
%template (fir_highpassf) fir_highpass<float>;
%template (fir_bandpassf) fir_bandpass<float>;
%template (fir_bandstopf) fir_bandstop<float>;

%template (FloatBesselFilter) BesselFilter<float>;
%template (FloatBesselLowPassFilter) BesselLowPassFilter<float>;
%template (FloatBesselHighPassFilter) BesselHighPassFilter<float>;
%template (FloatBesselBandPassFilter) BesselBandPassFilter<float>;
%template (FloatBesselBandStopFilter) BesselBandStopFilter<float>;

%template (FloatButterworthFilter) ButterworthFilter<float>;
%template (FloatButterworthLowPassFilter) ButterworthLowPassFilter<float>;
%template (FloatButterworthHighPassFilter) ButterworthHighPassFilter<float>;
%template (FloatButterworthBandPassFilter) ButterworthBandPassFilter<float>;
%template (FloatButterworthBandStopFilter) ButterworthBandStopFilter<float>;

%template (FloatChevyshev1Filter) Chebyshev1Filter<float>;
%template (FloatChevyshev1LowPassFilter)  Chebyshev1LowPassFilter<float>;
%template (FloatChevyshev1HighPassFilter) Chebyshev1HighPassFilter<float>;
%template (FloatChevyshev1BandPassFilter) Chebyshev1BandPassFilter<float>;
%template (FloatChevyshev1BandStopFilter) Chebyshev1BandStopFilter<float>;

%template (FloatChevyshev2Filter) Chebyshev2Filter<float>;
%template (FloatChevyshev2LowPassFilter) Chebyshev2LowPassFilter<float>;
%template (FloatChevyshev2HighPassFilter) Chebyshev2HighPassFilter<float>;
%template (FloatChevyshev2BandPassFilter) Chebyshev2BandPassFilter<float>;
%template (FloatChevyshev2BandStopFilter) Chebyshev2BandStopFilter<float>;

%constant int audio_sample_unknown = kfr::audio_sample_type::unknown;
%constant int audio_sample_i8  = kfr::audio_sample_type::i8;
%constant int audio_sample_i16 = kfr::audio_sample_type::i16;
%constant int audio_sample_i24 = kfr::audio_sample_type::i24;
%constant int audio_sample_i32 = kfr::audio_sample_type::i32;
%constant int audio_sample_i64 = kfr::audio_sample_type::i64;
%constant int audio_sample_f32 = kfr::audio_sample_type::f32;
%constant int audio_sample_f64 = kfr::audio_sample_type::f64;


%template(interleavef) do_interleave<float>;
%template(deinterleavef) do_deinterleave<float>; 

%template(interleaved) do_interleave<double>;
%template(deinterleaved) do_deinterleave<double>; 

%template(bessel_lowpassf) bessel_lowpass<float>;
%template(bessel_highpassf) bessel_highpass<float>;
%template(bessel_bandpassf) bessel_bandpass<float>;
%template(bessel_bandstopf) bessel_bandstop<float>;

%template(butterworth_lowpassf) butterworth_lowpass<float>;
%template(butterworth_highpassf) butterworth_highpass<float>;
%template(butterworth_bandpassf) butterworth_bandpass<float>;
%template(butterworth_bandstopf) butterworth_bandstop<float>;

%template(chebyshev1_lowpassf) chebyshev1_lowpass<float>;
%template(chebyshev1_highpassf) chebyshev1_highpass<float>;
%template(chebyshev1_bandpassf) chebyshev1_bandpass<float>;
%template(chebyshev1_bandstopf) chebyshev1_bandstop<float>;

%template(chebyshev2_lowpassf)  chebyshev2_lowpass<float>;
%template(chebyshev2_highpassf) chebyshev2_highpass<float>;
%template(chebyshev2_bandpassf) chebyshev2_bandpass<float>;
%template(chebyshev2_bandstopf) chebyshev2_bandstop<float>;

%template (convolve) convolve<float>;
%template (FloatConvolveFilter) ConvolveFilter<float>;


%template(FloatWavReader) WavReader<f32>;
%template(DoubleWavReader) WavReader<f64>;

%template(FloatWavWriter) WavWriter<f32>;
%template(DoubleWavWriter) WavWriter<f64>;

%template(FloatMp3Reader) MP3Reader<f32>;
%template(DoubleMp3Reader) MP3Reader<f64>;

%template(FloatFlacReader) FlacReader<f32>;
%template(DoubleFlacReader) FlacReader<f64>;

%template (randomf) random<f32>;
%template (plot_savef) plot_save<f32>;
%template (plot_showf) plot_show<f32>;

%template (sinewavef) sinewave<f32>;
%template (squarewavef) squarewave<f32>;
%template (trianglewavef) trianglewave<f32>;
%template (sawtoothwavef) sawtoothwave<f32>;

%template (generate_sinef) generate_sin<f32>;
%template (generate_linearf) generate_linear<f32>;
%template (generate_expf) generate_exp<f32>;
%template (generate_exp2f) generate_exp2<f32>;
%template (generate_cossinf) generate_cossin<f32>;

%template (randomd) random<f64>;
%template (plot_saved) plot_save<f64>;
%template (plot_showd) plot_show<f64>;

%template (sinewaved) sinewave<f64>;
%template (squarewaved) squarewave<f64>;
%template (trianglewaved) trianglewave<f64>;
%template (sawtoothwaved) sawtoothwave<f64>;

%template (generate_sined) generate_sin<f64>;
%template (generate_lineard) generate_linear<f64>;
%template (generate_expd) generate_exp<f64>;
%template (generate_exp2d) generate_exp2<f64>;
%template (generate_cossind) generate_cossin<f64>;

%template (autocorrelatef) autocorrelate<float>;
%template (correlatef) correlate<float>;
%template (FloatDCTPlan) DCTPlan<float>;
%template (FloatDFTPlan) DFTPlan<float>;
%template (FloatDFTRealPlan) DFTRealPlan<float>;
%template (resamplef) resample<float>;
%template (convert_samplef) convert_sample<float>;
%template (amp_to_dBf) amp_to_dB<float>;

%template (FloatFIRFilter) FIRFilter<float>;
%template (FloatFIRBandpassFilter) FIRBandpassFilter<float>;
%template (FloatFIRLowpassFilter) FIRLowpassFilter<float>;
%template (FloatFIRHighpassFilter) FIRHighpassFilter<float>;
%template (FloatFIRBandstopFilter) FIRBandstopFilter<float>;

%template (fir_lowpassf) fir_lowpass<float>;
%template (fir_highpassf) fir_highpass<float>;
%template (fir_bandpassf) fir_bandpass<float>;
%template (fir_bandstopf) fir_bandstop<float>;

%template (FloatBesselFilter) BesselFilter<float>;
%template (FloatBesselLowPassFilter) BesselLowPassFilter<float>;
%template (FloatBesselHighPassFilter) BesselHighPassFilter<float>;
%template (FloatBesselBandPassFilter) BesselBandPassFilter<float>;
%template (FloatBesselBandStopFilter) BesselBandStopFilter<float>;

%template (FloatButterworthFilter) ButterworthFilter<float>;
%template (FloatButterworthLowPassFilter) ButterworthLowPassFilter<float>;
%template (FloatButterworthHighPassFilter) ButterworthHighPassFilter<float>;
%template (FloatButterworthBandPassFilter) ButterworthBandPassFilter<float>;
%template (FloatButterworthBandStopFilter) ButterworthBandStopFilter<float>;

%template (FloatChevyshev1Filter) Chebyshev1Filter<float>;
%template (FloatChevyshev1LowPassFilter)  Chebyshev1LowPassFilter<float>;
%template (FloatChevyshev1HighPassFilter) Chebyshev1HighPassFilter<float>;
%template (FloatChevyshev1BandPassFilter) Chebyshev1BandPassFilter<float>;
%template (FloatChevyshev1BandStopFilter) Chebyshev1BandStopFilter<float>;

%template (FloatChevyshev2Filter) Chebyshev2Filter<float>;
%template (FloatChevyshev2LowPassFilter) Chebyshev2LowPassFilter<float>;
%template (FloatChevyshev2HighPassFilter) Chebyshev2HighPassFilter<float>;
%template (FloatChevyshev2BandPassFilter) Chebyshev2BandPassFilter<float>;
%template (FloatChevyshev2BandStopFilter) Chebyshev2BandStopFilter<float>;
