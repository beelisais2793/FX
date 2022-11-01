#ifndef GOLDENROCEKEFELLER_FAST_ADDITIVE_IMPLEMENTATIONS_RECURSIVE_HPP
#define GOLDENROCEKEFELLER_FAST_ADDITIVE_IMPLEMENTATIONS_RECURSIVE_HPP
#include <cstddef>
#include <vector>
#include <cmath>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <array>

#include "common.hpp"


namespace goldenrockefeller{ namespace fast_additive_comparison{

    template <typename sample_type, typename operand_type, std::size_t N_OPERANDS_PER_BLOCK> 
    class MagicCircleOscillator{
        static_assert(sizeof(operand_type) >= sizeof(sample_type), "The operand type size must be the same size as sample type");
        static_assert((sizeof(operand_type) % sizeof(sample_type)) == 0, "The operand type size must be a multiple of size as sample type");
        static_assert(N_OPERANDS_PER_BLOCK >= 1, "The operand block length must be positive");

        using size_t = std::size_t;
        using vector_type = typename std::vector<sample_type>;
        using vector_iterator_type = typename std::vector<sample_type>::iterator;

        static constexpr size_t N_SAMPLES_PER_OPERAND = sizeof(operand_type) / sizeof(sample_type);
        static constexpr size_t N_SAMPLES_PER_BLOCK = N_OPERANDS_PER_BLOCK * sizeof(operand_type) / sizeof(sample_type);

        operand_type osc_block_param;

        vector_type osc_block;
        vector_iterator_type osc_block_it;
        vector_iterator_type osc_block_safe_end_it;
        vector_iterator_type osc_block_safe_begin_it;

        vector_type co_osc_block;

        static inline void set_osc_operand(sample_type& osc_ref, const sample_type& phase_ref, const operand_type& ampl_operand) {
            operand_type osc_operand;
            operand_type phase_operand;
            load(&phase_ref, phase_operand); 
            osc_operand  = ampl_operand * approx_cos_deg_14(phase_operand);
            store(&osc_ref, osc_operand);
        }
    
        static inline void progress_osc_operand(sample_type& osc_ref, sample_type& co_osc_ref, const operand_type& osc_block_param) {
            operand_type osc_operand;
            operand_type co_osc_operand;
            load(&osc_ref, osc_operand); 
            load(&co_osc_ref, co_osc_operand); 
            osc_operand  = osc_operand - osc_block_param * co_osc_operand; 
            co_osc_operand  = co_osc_operand + osc_block_param * osc_operand; 
            store(&osc_ref, osc_operand);
            store(&co_osc_ref, co_osc_operand); 
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
                MagicCircleOscillator::init_phase_block(phase_block, freq, phase);
                return phase_block;
            }

            static vector_type new_osc_block(sample_type freq, sample_type ampl, sample_type phase) {
                auto phase_block = new_phase_block(freq, phase);
                vector_type osc_block(phase_block.size() + N_SAMPLES_PER_OPERAND, 0.);      

                operand_type ampl_operand(ampl);

                for (size_t i = 0; i < N_SAMPLES_PER_BLOCK; i += N_SAMPLES_PER_OPERAND) {
                    MagicCircleOscillator::set_osc_operand(
                        osc_block[i + N_SAMPLES_PER_OPERAND], 
                        phase_block[i],
                        ampl_operand
                    );
                }

                return osc_block;
            }


            MagicCircleOscillator() : MagicCircleOscillator(sample_type(0), sample_type(0), sample_type(0)) {}

            MagicCircleOscillator(sample_type freq, sample_type ampl, sample_type phase) :
                osc_block_param(2. * approx_cos_deg_14(wrap_phase(wrap_phase(pi<sample_type>() * freq * N_SAMPLES_PER_BLOCK) - 0.5 * pi<sample_type>()))),
                osc_block(MagicCircleOscillator::new_osc_block(freq, ampl, phase)),
                osc_block_safe_end_it(osc_block.begin() + N_SAMPLES_PER_BLOCK),
                osc_block_safe_begin_it(osc_block.begin() + N_SAMPLES_PER_OPERAND),
                osc_block_it(osc_block.begin()+N_SAMPLES_PER_OPERAND),
                co_osc_block(MagicCircleOscillator::new_osc_block(freq, ampl,phase + wrap_phase(pi<sample_type>() * freq * N_SAMPLES_PER_BLOCK) - 0.5 * pi<sample_type>()))
            {}

            void reset(sample_type freq, sample_type ampl, sample_type phase) {
                this->osc_block_param = 2. * approx_cos_deg_14(wrap_phase(wrap_phase(pi<sample_type>() * freq * N_SAMPLES_PER_BLOCK) - 0.5 * pi<sample_type>()));
                this->osc_block = MagicCircleOscillator::new_osc_block(freq, ampl, phase);
                this->osc_block_safe_end_it = osc_block.begin() + N_SAMPLES_PER_BLOCK;
                this->osc_block_safe_begin_it = osc_block.begin() + N_SAMPLES_PER_OPERAND;
                this->osc_block_it = osc_block.begin()+N_SAMPLES_PER_OPERAND;
                this->co_osc_block = MagicCircleOscillator::new_osc_block(freq, ampl, phase + wrap_phase(pi<sample_type>() * freq * N_SAMPLES_PER_BLOCK) - 0.5 * pi<sample_type>());
            }

            void update_osc_block(size_t sample_offset) {
                operand_type last_osc_operand;
                
                load(&(*this->osc_block_safe_end_it), last_osc_operand);
                store(this->osc_block.data(), last_osc_operand);

                for (size_t i = 0; i < N_SAMPLES_PER_BLOCK; i += N_SAMPLES_PER_OPERAND) {
                    MagicCircleOscillator::set_osc_operand(
                        this->osc_block[i + N_SAMPLES_PER_OPERAND], 
                        this->co_osc_block[+ N_SAMPLES_PER_OPERAND],
                        this->osc_block_param
                    );
                }

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
                            this->update_osc_block(size_t(this->osc_block_it - this->osc_block_safe_end_it));
                        }

                        *signal_it += *this->osc_block_it;
                        ++this->osc_block_it;
                    }
                } 

                else { // it is safe to vectorize
                    auto signal_safe_end_it = signal_end_it - N_SAMPLES_PER_OPERAND;
                    operand_type signal_operand_last;
                    load(&(*signal_safe_end_it), signal_operand_last);

                    auto signal_it = signal_begin_it;
                    for (; signal_it < signal_safe_end_it; signal_it += N_SAMPLES_PER_OPERAND) {

                        if (this->osc_block_it >  this->osc_block_safe_end_it) {
                            this->update_osc_block(size_t(this->osc_block_it - this->osc_block_safe_end_it));
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
                        this->update_osc_block(size_t(this->osc_block_it - this->osc_block_safe_end_it));
                    }

                    operand_type osc_operand;
                    load(&(*this->osc_block_it), osc_operand);

                    signal_operand_last = signal_operand_last + osc_operand;

                    store(&(*signal_safe_end_it), signal_operand_last);

                    this->osc_block_it += N_SAMPLES_PER_OPERAND;
                }
            }
        // public
    };

}}



#endif