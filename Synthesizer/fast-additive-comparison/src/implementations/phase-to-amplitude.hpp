#ifndef GOLDENROCEKEFELLER_FAST_ADDITIVE_IMPLEMENTATIONS_PHASE_TO_AMPLITUDE_HPP
#define GOLDENROCEKEFELLER_FAST_ADDITIVE_IMPLEMENTATIONS_PHASE_TO_AMPLITUDE_HPP
#include <cstddef>
#include <vector>
#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <array>

#include "common.hpp"


namespace goldenrockefeller{ namespace fast_additive_comparison{
    template <typename sample_type>
    class SimpleExactSineOscillator {
        using size_t = std::size_t;
        using vector_type = typename std::vector<sample_type>;

        sample_type freq;
        sample_type ampl;
        sample_type phase;
        sample_type delta_phase;
        public:
            typedef sample_type sample_type;

            SimpleExactSineOscillator() : SimpleExactSineOscillator(sample_type(0), sample_type(0), sample_type(0)) {}

            SimpleExactSineOscillator(sample_type freq, sample_type ampl, sample_type phase) :
                freq(freq),
                ampl(ampl),
                phase(phase),
                delta_phase(wrap_phase_offset(tau<sample_type>() * freq))
            {}

            void reset(sample_type freq, sample_type ampl, sample_type phase) {
                this->freq = freq;
                this->ampl = ampl;
                this->phase = phase;
                this->delta_phase = wrap_phase_offset(tau<sample_type>() * freq);
            }

            template<typename iterator_type>
            void progress_and_add(iterator_type signal_begin_it, iterator_type signal_end_it) {
                
                for (auto signal_it = signal_begin_it; signal_it < signal_end_it; ++signal_it) {
                    *signal_it += ampl * cos(this->phase);
                    this->phase += this->delta_phase;
                    this->phase = wrap_phase_bounded(this->phase);
                }
            }
        // end public
    };

    template <typename sample_type>
    struct ExactCosineCalculator {

        template <typename operand_type>
        static inline operand_type cos(const operand_type& x) {
            static_assert(sizeof(operand_type) >= sizeof(sample_type), "The operand type size must be the same size as sample type");
            static_assert((sizeof(operand_type) % sizeof(sample_type)) == 0, "The operand type size must be a multiple of size as sample type");

            static constexpr size_t N_SAMPLES_PER_OPERAND = sizeof(operand_type) / sizeof(sample_type);

            operand_type y;
            const sample_type* x_ptr = reinterpret_cast<const sample_type*>(&x);
            sample_type* y_ptr = reinterpret_cast<sample_type*>(&y);

            for (size_t i = 0; i < N_SAMPLES_PER_OPERAND; i++) {
                y_ptr[i] = goldenrockefeller::fast_additive_comparison::cos(sample_type(x_ptr[i]));
            }
            return y;
        }
    };

    
    struct ApproxCos14Calculator {
        template <typename operand_type>
        static inline operand_type cos(const operand_type& x) {
            return approx_cos_deg_14(x);
        }
    };

    struct ApproxCos10Calculator {
        template <typename operand_type>
        static inline operand_type cos(const operand_type& x) {
            return approx_cos_deg_10(x);
        }
    };
    
    struct IdentityCalculator {
        template <typename operand_type>
        static inline operand_type cos(const operand_type& x) {
            return x;
        }
    };


    #include "lookup_table.h"
    
    template <typename sample_type>
    sample_type lookup_cos (const sample_type& x) {
        return lookup_table[ size_t( x)];
    }

    template <typename sample_type>
    struct LookupCalculator {
        template <typename operand_type>
        static inline operand_type cos(const operand_type& x) {
            static_assert(sizeof(operand_type) >= sizeof(sample_type), "The operand type size must be the same size as sample type");
            static_assert((sizeof(operand_type) % sizeof(sample_type)) == 0, "The operand type size must be a multiple of size as sample type");

            static constexpr size_t N_SAMPLES_PER_OPERAND = sizeof(operand_type) / sizeof(sample_type);

            
            constexpr double step_size = 1024. / tau<sample_type>();

            operand_type x_id;

            x_id = (x + operand_type(x < 0.) * tau<sample_type>()) * operand_type(step_size);

            operand_type y;
            const sample_type* x_id_ptr = reinterpret_cast<const sample_type*>(&x_id);
            sample_type* y_ptr = reinterpret_cast<sample_type*>(&y);

            for (size_t i = 0; i < N_SAMPLES_PER_OPERAND; i++) {
                y_ptr[i] = lookup_cos(sample_type(x_id_ptr[i]));
            }
            return y;
        }
    };

    template <typename sample_type, typename operand_type, std::size_t N_OPERANDS_PER_BLOCK, typename CosineCalculatorT> 
    class SineOscillator{
        static_assert(sizeof(operand_type) >= sizeof(sample_type), "The operand type size must be the same size as sample type");
        static_assert((sizeof(operand_type) % sizeof(sample_type)) == 0, "The operand type size must be a multiple of size as sample type");
        static_assert(N_OPERANDS_PER_BLOCK >= 1, "The operand block length must be positive");

        using size_t = std::size_t;
        using vector_type = typename std::vector<sample_type>;
        using vector_iterator_type = typename std::vector<sample_type>::iterator;

        static constexpr size_t N_SAMPLES_PER_OPERAND = sizeof(operand_type) / sizeof(sample_type);
        static constexpr size_t N_SAMPLES_PER_BLOCK = N_OPERANDS_PER_BLOCK * sizeof(operand_type) / sizeof(sample_type);

        sample_type freq;
        operand_type ampl_operand;

        operand_type delta_phase_per_block;

        vector_type osc_block;
        vector_type phase_block;
        vector_iterator_type osc_block_it;
        vector_iterator_type osc_block_safe_end_it;
        vector_iterator_type osc_block_safe_begin_it;

        static inline void progress_phase_operand(sample_type& phase_ref, const operand_type& delta_phase_per_block) {
            operand_type phase_operand;
            load(&phase_ref, phase_operand);
            phase_operand += delta_phase_per_block;
            phase_operand = wrap_phase_bounded(phase_operand);
            store(&phase_ref, phase_operand);
        }

        static inline void update_osc_operand(sample_type& osc_ref, const sample_type& phase_ref, const operand_type& ampl_operand) {
            operand_type osc_operand;
            operand_type phase_operand;
            load(&phase_ref, phase_operand); 
            osc_operand  = ampl_operand * CosineCalculatorT::cos(phase_operand);
            store(&osc_ref, osc_operand);
        }

    
        public:
            typedef sample_type sample_type;

            static void init_phase_block(vector_type& phase_block, sample_type freq, sample_type phase) {
                if (phase_block.size() !=  N_SAMPLES_PER_BLOCK) {
                    std::ostringstream msg;
                    msg << "The phase block size "
                        << "(phase_block.size() = " << phase_block.size() << ") "
                        << "must be equal to the number of samples per block "
                        << "(N_SAMPLES_PER_BLOCK = " << N_SAMPLES_PER_BLOCK << ") ";
                    throw std::invalid_argument(msg.str());
                }

                auto delta_phase_per_sample = wrap_phase_offset(tau<sample_type>() * freq);
                phase = wrap_phase(phase);

                // Fill the first phase operand
                for (auto it = phase_block.begin(); it < phase_block.begin() + N_SAMPLES_PER_OPERAND; ++it){
                    *it = phase;
                    phase += delta_phase_per_sample;
                    phase = wrap_phase_bounded(phase);
                }

                operand_type delta_phase_per_operand(wrap_phase_offset(tau<sample_type>() * freq * N_SAMPLES_PER_OPERAND));

                // Fill the first phase block
                operand_type prev_phase_operand;
                load(phase_block.data(), prev_phase_operand);
                for (size_t i = N_SAMPLES_PER_OPERAND; i < N_SAMPLES_PER_BLOCK; i += N_SAMPLES_PER_OPERAND) {
                    operand_type phase_operand;
                    load(&phase_block[i], phase_operand);
                    phase_operand = wrap_phase_bounded(prev_phase_operand + delta_phase_per_operand);
                    prev_phase_operand = phase_operand;
                    store(&phase_block[i], phase_operand);
                }  

            }

            static vector_type new_phase_block(sample_type freq, sample_type phase) {

                vector_type phase_block(N_SAMPLES_PER_BLOCK, 0.);                
                SineOscillator::init_phase_block(phase_block, freq, phase);
                return phase_block;
            }

            static vector_type new_osc_block(sample_type freq, sample_type ampl, sample_type phase) {
                auto phase_block = new_phase_block(freq, phase);
                vector_type osc_block(phase_block.size() + N_SAMPLES_PER_OPERAND, 0.);      

                operand_type ampl_operand(ampl);

                for (size_t i = 0; i < N_SAMPLES_PER_BLOCK; i += N_SAMPLES_PER_OPERAND) {
                    SineOscillator::update_osc_operand(
                        osc_block[i + N_SAMPLES_PER_OPERAND], 
                        phase_block[i],
                        ampl_operand
                    );
                }

                return osc_block;
            }

            SineOscillator() : SineOscillator(sample_type(0), sample_type(0), sample_type(0)) {}

            SineOscillator(sample_type freq, sample_type ampl, sample_type phase) :
                freq(freq),
                ampl_operand(ampl),
                delta_phase_per_block(wrap_phase_offset(tau<sample_type>() * freq * N_SAMPLES_PER_BLOCK)),
                phase_block(SineOscillator::new_phase_block(freq, phase)),
                osc_block(SineOscillator::new_osc_block(freq, ampl, phase)),
                osc_block_safe_end_it(osc_block.begin() + N_SAMPLES_PER_BLOCK),
                osc_block_safe_begin_it(osc_block.begin() + N_SAMPLES_PER_OPERAND),
                osc_block_it(osc_block.begin()+N_SAMPLES_PER_OPERAND)
            {}

            void reset(sample_type freq, sample_type ampl, sample_type phase) {
                this->freq = 0.*freq;
                this->ampl_operand = operand_type(ampl);
                this->delta_phase_per_block = operand_type(wrap_phase_offset(tau<sample_type>() * freq * N_SAMPLES_PER_BLOCK));
                SineOscillator::init_phase_block(this->phase_block, freq, phase);
                this->update_osc_block();
                this->osc_block_safe_end_it = this->osc_block.begin() + N_SAMPLES_PER_BLOCK;
                this->osc_block_safe_begin_it = this->osc_block.begin() + N_SAMPLES_PER_OPERAND;
                this->osc_block_it = this->osc_block.begin() + N_SAMPLES_PER_OPERAND;
            }

            void progress_phase_block() {
                for (size_t i = 0; i < N_SAMPLES_PER_BLOCK; i += N_SAMPLES_PER_OPERAND) {
                    SineOscillator::progress_phase_operand(this->phase_block[i], this->delta_phase_per_block);
                }
            }

            void update_osc_block() {
                for (size_t i = 0; i < N_SAMPLES_PER_BLOCK; i += N_SAMPLES_PER_OPERAND) {
                    SineOscillator::update_osc_operand(
                        this->osc_block[i + N_SAMPLES_PER_OPERAND], 
                        this->phase_block[i],
                        this->ampl_operand
                    );
                }
            }

            void prorgess_osc_block(size_t sample_offset) {
                operand_type last_osc_operand;
                
                load(&(*this->osc_block_safe_end_it), last_osc_operand);
                store(this->osc_block.data(), last_osc_operand);

                this->progress_phase_block();
                this->update_osc_block();

                this->osc_block_it = this->osc_block.begin() + sample_offset;
            }

            template<typename iterator_type>
            void progress_and_add(iterator_type signal_begin_it, iterator_type signal_end_it)    {
                
                if (signal_end_it < signal_begin_it) {
                    return;
                }

                if  (signal_end_it - signal_begin_it < N_SAMPLES_PER_OPERAND) { // it is not safe to vectorize
                    for (auto signal_it = signal_begin_it; signal_it < signal_end_it; ++signal_it) {
                        if (this->osc_block_it >  this->osc_block_safe_end_it) {
                            this->prorgess_osc_block(size_t(this->osc_block_it - this->osc_block_safe_end_it));
                        }

                        *signal_it += *this->osc_block_it;
                        ++this->osc_block_it;
                    }
                } 

                else { // it is safe to vectorize
                    auto signal_safe_end_it = signal_end_it - N_SAMPLES_PER_OPERAND;
                    operand_type last_signal_operand;
                    load(&(*signal_safe_end_it), last_signal_operand);

                    auto signal_it = signal_begin_it;
                    for (; signal_it < signal_safe_end_it; signal_it += N_SAMPLES_PER_OPERAND) {

                        if (this->osc_block_it >  this->osc_block_safe_end_it) {
                            this->prorgess_osc_block(size_t(this->osc_block_it - this->osc_block_safe_end_it));
                        }
                        
                        operand_type signal_operand;
                        operand_type osc_operand;
                        load(&(*signal_it), signal_operand);
                        load(&(*this->osc_block_it), osc_operand);

                        signal_operand += osc_operand;

                        store(&(*signal_it), signal_operand);

                        this->osc_block_it += N_SAMPLES_PER_OPERAND;
                    }

                    this->osc_block_it -= signal_it - signal_safe_end_it;

                    if (this->osc_block_it >  this->osc_block_safe_end_it) {
                        this->prorgess_osc_block(size_t(this->osc_block_it - this->osc_block_safe_end_it));
                    }

                    operand_type osc_operand;
                    load(&(*this->osc_block_it), osc_operand);

                    last_signal_operand = last_signal_operand + osc_operand;

                    store(&(*signal_safe_end_it), last_signal_operand);

                    this->osc_block_it += N_SAMPLES_PER_OPERAND;
                }
            }
        // public
    };
}}



#endif