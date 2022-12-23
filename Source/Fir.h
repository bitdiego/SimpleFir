/*
  ==============================================================================

    Fir.h
    Created: 29 Nov 2022 10:20:57am
    Author:  bitdiego

  ==============================================================================
*/

#pragma once

#include "DelayLine.h"
#include <vector>
#include <memory>

template<class T>
class Fir {
public:
 explicit Fir(std::shared_ptr<DelayLine<T>> delay_line, T gain = 0)
     : m_delay_line(std::move(delay_line)), m_gain(gain) {
    }

    void set_gain(T gain) {
        m_gain = gain;
    }

    T get_gain() const {
        return m_gain;
    }

    T get_fir_sample(const T& input) {
        T output = input + m_gain * m_delay_line->compute_sample(input);
        return output;
    }

private:
    T m_gain{ 0.0 };
    std::shared_ptr<DelayLine<T>> m_delay_line;
};