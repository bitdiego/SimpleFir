/*
  ==============================================================================

    DelayLine.h
    Created: 29 Nov 2022 10:20:45am
    Author:  bitdiego

  ==============================================================================
*/

#pragma once

#include <vector>
#include "GlobalSettings.h"

template <class T>
class DelayLine {
public:
    DelayLine() {
        this->m_buffer.resize(max_delay_in_samples);
    }
    /*DelayLine(std::size_t size) {
        this->m_buffer.resize(size);
    }*/
    ~DelayLine() {
        m_buffer.clear();
    }
    void set_delay(int d) {
        if (d < min_delay_in_samples || d > max_delay_in_samples) return;
        m_delay = d;
       //int xx = static_cast<int>(d);
        m_read_index = m_write_index - d;
        if (m_read_index < 0) { 
            m_read_index += static_cast<int>(m_buffer.size());//WARNING::: always use signed and convert unsigned -> signed
        }
    }

    int get_delay() const {
        return m_delay;
    }
  

    T compute_sample(const T& input) {
        m_buffer[m_write_index++] = input;
        T output = m_buffer[m_read_index++];
        if (m_read_index >= std::ssize(m_buffer)) {
          m_read_index -= std::ssize(m_buffer);
        }
        if (m_write_index >= std::ssize(m_buffer)) {
          m_write_index -= std::ssize(m_buffer);
        }

        return output;
    }

    std::vector<T> get_out_buffer() const {
        return m_buffer;
    }

private:
    int m_delay{ 0 };
    int m_read_index{ 0 };
    int m_write_index{ 0 };
    std::vector<T> m_buffer;

};