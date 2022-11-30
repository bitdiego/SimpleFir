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

    //    DP: is it ok?
    /*Fir(std::shared_ptr<DelayLineExt<T>> delay_line) : m_delay_line(delay_line) {

    }

    Fir(std::shared_ptr<DelayLineExt<T>> delay_line, double gain) : m_delay_line(delay_line), m_gain(gain) {

    }

    ~Fir()
    {

    }
    */
    Fir(DelayLine<T>* delay_line) : m_delay_line(delay_line) {

    }
    Fir(DelayLine<T>* delay_line, T gain) : m_delay_line(delay_line), m_gain(gain) {

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
    DelayLine<T>* m_delay_line;

    //DP: is it ok?
    //std::shared_ptr<DelayLineExt<T>> m_delay_line;
};