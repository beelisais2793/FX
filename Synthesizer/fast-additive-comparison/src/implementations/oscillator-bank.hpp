#ifndef GOLDENROCEKEFELLER_FAST_ADDITIVE_IMPLEMENTATIONS_OSCILLATOR_BANK_HPP
#define GOLDENROCEKEFELLER_FAST_ADDITIVE_IMPLEMENTATIONS_OSCILLATOR_BANK_HPP
#include <cstddef>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "common.hpp"


namespace goldenrockefeller{ namespace fast_additive_comparison{
    template<typename OscillatorT>
    class OscillatorBank {
        public:
            using sample_type = typename OscillatorT::sample_type;

        private:
            using size_t = std::size_t;
            using vector_type = typename std::vector<OscillatorT>;

            vector_type oscs;

        public:
            typedef sample_type sample_type;

            OscillatorBank() : OscillatorBank(0) {}
            OscillatorBank(size_t n_oscs) : oscs(n_oscs) {}

            void _reset_osc(size_t osc_id, sample_type freq, sample_type ampl, sample_type phase) {
                this->oscs[osc_id].reset(freq, ampl, phase);
            }

            void reset_osc(size_t osc_id, sample_type freq, sample_type ampl, sample_type phase) {
                if (osc_id >=  oscs.size()) {
                    std::ostringstream msg;
                    msg << "A valid oscilator id "
                        << "(osc_id= " << osc_id << ") "
                        << "must less than the number of oscilators "
                        << "(oscs.size() = " << oscs.size() << ") ";
                    throw std::invalid_argument(msg.str());
                }

                this->_reset_osc(osc_id, freq, ampl, phase);
            }

            template <typename iterator_type>
            void progress_and_add(iterator_type signal_begin_it, iterator_type signal_end_it) {
                for (OscillatorT& osc: oscs) {
                    osc.progress_and_add(signal_begin_it, signal_end_it);
                }
            }
        // public
    };
}}

#endif