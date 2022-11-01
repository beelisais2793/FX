#pragma once 
#include <cassert>
#include <vector>

#include "DiodeLadder.hpp"
#include "Korg35HPF.hpp"
#include "Korg35LPF.hpp"
#include "MoogHalfLadder.hpp"
#include "MoogLadder.hpp"
#include "Oberheim.hpp"


namespace Filters
{
struct DiodeLadderFilter
{
    DiodeLadder::DiodeLadder * filter;

    DiodeLadderFilter(float sample_rate) {
        filter = new DiodeLadder::DiodeLadder();
        filter->init(sample_rate);
        assert(filter != NULL);
    }
    ~DiodeLadderFilter() {
        if(filter) delete filter;
    }

    void Clear() { filter->clear(); }

    void Process(size_t n, const std::vector<float> & input, std::vector<float> & output) {
        output.resize(n);
        filter->process(input.data(),output.data(),n);
    }

    void Process(float * input, float * output, size_t n) {
        filter->process(input,output,n);
    }

    float GetCutoff() { return filter->get_cutoff(); }
    float GetQ() { return filter->get_q(); }

    void SetCutoff(float c) { filter->set_cutoff(c); }
    void SetQ(float q) { filter->set_q(q); }

};

struct Korg35HPFilter {
    Korg35::Korg35HPF * filter;
    Korg35HPFilter(float sample_rate) {
        filter = new Korg35::Korg35HPF();
        filter->init(sample_rate);
        assert(filter != NULL);
    }
    ~Korg35HPFilter() {
        if(filter) delete filter;
    }

    void Clear() { filter->clear(); }

    void Process(size_t n, const std::vector<float> & input, std::vector<float> & output) {
        output.resize(n);
        filter->process(input.data(),output.data(),n);
    }

    void Process(float * input, float * output, size_t n) {
        filter->process(input,output,n);
    }

    float GetCutoff() { return filter->get_cutoff(); }
    float GetQ() { return filter->get_q(); }

    void SetCutoff(float c) { filter->set_cutoff(c); }
    void SetQ(float q) { filter->set_q(q); }
};

struct Korg35LPFilter {
    Korg35::Korg35LPF * filter;
    Korg35LPFilter(float sample_rate) {
        filter = new Korg35::Korg35LPF();
        filter->init(sample_rate);
        assert(filter != NULL);
    }
    ~Korg35LPFilter() {
        if(filter) delete filter;
    }

    void Clear() { filter->clear(); }

    void Process(size_t n, const std::vector<float> & input, std::vector<float> & output) {
        output.resize(n);
        filter->process(input.data(),output.data(),n);
    }

    void Process(float * input, float * output, size_t n) {
        filter->process(input,output,n);
    }

    float GetCutoff() { return filter->get_cutoff(); }
    float GetQ() { return filter->get_q(); }

    void SetCutoff(float c) { filter->set_cutoff(c); }
    void SetQ(float q) { filter->set_q(q); }
};


struct MoogHalfLadderFilter
{
    Moog::MoogHalfLadder * filter;

    MoogHalfLadderFilter(float sample_rate) {
        filter = new Moog::MoogHalfLadder();
        filter->init(sample_rate);
        assert(filter != NULL);
    }
    ~MoogHalfLadderFilter() {
        if(filter) delete filter;
    }

    void Clear() { filter->clear(); }

    void Process(size_t n, const std::vector<float> & input, std::vector<float> & output) {
        output.resize(n);
        filter->process(input.data(),output.data(),n);
    }

    void Process(float * input, float * output, size_t n) {
        filter->process(input,output,n);
    }

    float GetCutoff() { return filter->get_cutoff(); }
    float GetQ() { return filter->get_q(); }

    void SetCutoff(float c) { filter->set_cutoff(c); }
    void SetQ(float q) { filter->set_q(q); }

};

struct MoogFilter
{
    Moog::MoogLadder * filter;

    MoogFilter(float sample_rate) {
        filter = new Moog::MoogLadder();
        filter->init(sample_rate);
        assert(filter != NULL);
    }
    ~MoogFilter() {
        if(filter) delete filter;
    }

    void Clear() { filter->clear(); }

    void Process(size_t n, const std::vector<float> & input, std::vector<float> & output) {
        output.resize(n);
        filter->process(input.data(),output.data(),n);
    }

    void Process(float * input, float * output, size_t n) {
        filter->process(input,output,n);
    }

    float GetCutoff() { return filter->get_cutoff(); }
    float GetQ() { return filter->get_q(); }

    void SetCutoff(float c) { filter->set_cutoff(c); }
    void SetQ(float q) { filter->set_q(q); }

};

struct OberheimFilter
{
    Oberheim::Oberheim * filter;

    OberheimFilter(float sample_rate) {
        filter = new Oberheim::Oberheim();
        filter->init(sample_rate);
        assert(filter != NULL);
    }
    ~OberheimFilter() {
        if(filter) delete filter;
    }

    void Clear() { filter->clear(); }

    void Process(size_t n, const std::vector<float> & input, std::vector<float> & output1, std::vector<float> & output2, std::vector<float> & output3, std::vector<float> & output4) {
        output1.resize(n);
        output2.resize(n);
        output3.resize(n);
        output4.resize(n);
        filter->process(input.data(),output1.data(),output2.data(),output3.data(),output4.data(),n);
    }

    void Process(float * input, float * out1, float * out2, float * out3, float * out4, size_t n) {
        filter->process(input,out1,out2,out3,out4,n);
    }

    float GetCutoff() { return filter->get_cutoff(); }
    float GetQ() { return filter->get_q(); }

    void SetCutoff(float c) { filter->set_cutoff(c); }
    void SetQ(float q) { filter->set_q(q); }

};
};
