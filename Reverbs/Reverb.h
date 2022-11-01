/*
  ==============================================================================

    Reverb.h
    Created: 23 May 2020 12:56:26pm
    Author:  mw

  ==============================================================================
*/

#pragma once
#include <exception>
#include <vector>
#include "RingBuffer.h"

class Delay
{
public:
    Delay(size_t max_size, float gain) :
        buffer_(RingBuffer(max_size))
    {
        setGain(gain);
    }

    float update(float sample)
    {
        float left = buffer_.get(0);
        float right = buffer_.get(1);
        float out = sample + gain_ * (left + right) / 2.f;
        buffer_.update(out);
        return out;
    }

    bool setGain(float gain) {
        if (gain > 1.f) {
            return false;
        }

        gain_ = gain;
        return true;
    }

    bool setLength(size_t length)
    {
        return buffer_.resize(length);
    }

private:
    RingBuffer buffer_;
    float gain_;
};

class TankVerb
{
public:
    TankVerb(size_t max_delay_number, float gain, size_t max_delay_length) :
        max_delay_number_(max_delay_number),
        current_num_delays_(max_delay_number),
        gain_(gain),
        max_delay_length_(max_delay_length)
    {
        for (size_t i = 0u; i < current_num_delays_; ++i) {
            delays_.emplace_back(max_delay_length, gain);
        }
    }

    void setNumDelays(size_t num_delays) 
    {
        current_num_delays_ = num_delays;
    }

    void setRoomSize(size_t room_size)
    {
        for (size_t i = 0u; i < current_num_delays_; ++i) {
            delays_[i].setLength(5*room_size + (i + 10) * room_size);
        }
    }

    void setGain(float gain)
    {
        for (size_t i = 0u; i < current_num_delays_; ++i) {
            delays_[i].setGain(gain);
        }
    }

    float update(float sample) 
    {   
        float sum = 0;
        for (size_t i = 0u; i < current_num_delays_; ++i) {
            sum += delays_[i].update(sample);
        }
        return sum / static_cast<float>(current_num_delays_);
    }

private:
    size_t current_num_delays_;
    size_t max_delay_number_;
    size_t max_delay_length_;
    float gain_;

    std::vector<Delay> delays_;
};