#pragma once

#ifndef DELAY_HPP
#define DELAY_HPP
#if defined(WIN32)
#include <string.h>
#endif

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"

#if defined(USE_VGL) && USE_VGL
#include "vgl/vgl_unique_ptr.hpp"
using vgl::unique_ptr;
#include "vgl/vgl_vector.hpp"
using vgl::vector;
#else
#include <vector>
#include <memory>
using std::vector;
using std::unique_ptr;
#endif

#define NUM_DELAY_LINES 2

//----------------------------------------------------------------------------
// Delay
//----------------------------------------------------------------------------

class Delay
{
    public:
        //----------------------------------------------------------------------------
        Delay(void)
        {
            init();
        }

        void init()
        {
            m_delay_index = 0;
            m_delay_time = 1;
            m_feedback = 0.0f;
            //m_delay_buffer = new float[g_delay_buffer_size]();
            //m_delay_buffer = new float[g_delay_buffer_size];
            for (int ii = 0; ii < g_delay_buffer_size; ++ii)
            {
                m_delay_buffer.emplace_back(0.0f);
            }
            //clear();
        }

        //----------------------------------------------------------------------------
        ~Delay(void)
        {
        }

        //----------------------------------------------------------------------------
        void setFeedback(float feedback)
        {
#if defined(DNLOAD_USE_LD)
            assert(feedback >= 0.0f && feedback <= 1.0f);
#endif
            m_feedback = feedback;
        }

        //----------------------------------------------------------------------------
        void setDelayTime(float delay_time)
        {
#if defined(DNLOAD_USE_LD)
            assert(delay_time >= 0.0f && delay_time <= 1.0f);
            if (delay_time < 0.0f || delay_time > 1.0f)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error(std::string("ERROR: Delay::setDelayTime: delay_time out of bounds [0.0-1.0]: ") + std::to_string(delay_time)));
            }
#endif
            setDelayTime(static_cast<int>(g_delay_buffer_size * delay_time));
        }

        //----------------------------------------------------------------------------
        void setDelayTime(int delay_time)
        {
            m_delay_time = delay_time;
            if (m_delay_time < 1)
            {
                m_delay_time = 1;
            }
            clear();
        }

        //----------------------------------------------------------------------------
        float getSampleAt(int index)
        {
#if defined(DNLOAD_USE_LD)
            assert(index >= 0 && index < g_delay_buffer_size);
#endif
            return m_delay_buffer[index % m_delay_time];
        }

        //----------------------------------------------------------------------------
        float getSampleDelayedBy(int delay_in_samples)
        {
            int index = (m_delay_index - delay_in_samples) % m_delay_time;

            if (index < 0)
            {
                while (index < 0)
                {
                    index += m_delay_time;
                }
            }

#if defined(DNLOAD_USE_LD)
            assert(index >= 0 && index < g_delay_buffer_size);
#endif
            return m_delay_buffer[index];
        }

        //----------------------------------------------------------------------------
        // This version gets a linearly interpolated sample
        float getSampleDelayedBy(float delay_in_samples)
        {
            int whole = static_cast<int>(delay_in_samples);
            float frac = fmodf(delay_in_samples, 1.0f);
            float sample1 = getSampleDelayedBy(whole);
            float sample2 = getSampleDelayedBy(++whole);
            return ((1.0f - frac) * sample1) + (frac * sample2);
        }

        //----------------------------------------------------------------------------
        void write(float in)
        {
#if defined(DNLOAD_USE_LD)
            assert(m_delay_index >= 0 && m_delay_index < g_delay_buffer_size);
            if (m_delay_index < 0 || m_delay_index > g_delay_buffer_size)
            {
                std::ostringstream sstr;
                sstr << "m_delay_index out of bounds: " << m_delay_index << std::endl;
                BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
            }
#endif
            m_delay_buffer[m_delay_index] = in;

            ++m_delay_index;
            if (m_delay_index >= m_delay_time)
            {
                m_delay_index = 0;
            }
        }

        //----------------------------------------------------------------------------
        float read()
        {
#if defined(DNLOAD_USE_LD)
            if (m_delay_index < 0 || m_delay_index > g_delay_buffer_size)
            {
                std::ostringstream sstr;
                sstr << "m_delay_index out of bounds: " << m_delay_index << std::endl;
                BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
            }
            assert(m_delay_index >= 0 && m_delay_index < g_delay_buffer_size);
#endif
            return m_delay_buffer[m_delay_index];
        }

        //----------------------------------------------------------------------------
        void clear()
        {
            //memset(m_delay_buffer, 0, g_delay_buffer_size * sizeof(float));
            //m_delay_buffer.clear();
            for (int ii = 0; ii < g_delay_buffer_size; ++ii)
            {
                m_delay_buffer[ii] = 0.0f;
            }
        }

        //----------------------------------------------------------------------------
        float process(float in)
        {
            float out = read();
            in += (out * m_feedback);
            common::add_dc(in);
            write(in);
            return out;
        }

    private:
        vector<float> m_delay_buffer;
        int m_delay_index;
        float m_feedback;
        int m_delay_time;
};


//----------------------------------------------------------------------------
// AllPass
//----------------------------------------------------------------------------

class AllPass
{
    public:
        //----------------------------------------------------------------------------
        AllPass(void)
        {
            m_delay_line.emplace_back(new Delay());
            m_feedback = 0.0f;
            m_change_sign = false;
        }

        //----------------------------------------------------------------------------
        ~AllPass(void)
        {
        }

        //----------------------------------------------------------------------------
        void setFeedback(float feedback)
        {
#if defined(DNLOAD_USE_LD)
            assert(feedback >= 0.0f && feedback <= 1.0f);
#endif
            m_feedback = common::cclampf(feedback, 0.0f, 1.0f);
        }

        //----------------------------------------------------------------------------
        void setDelayTime(int delay_time)
        {
            m_delay_line[0]->setDelayTime(delay_time);
        }

        //----------------------------------------------------------------------------
        void setChangeSign(bool change_sign)
        {
            m_change_sign = change_sign;
        }

        //----------------------------------------------------------------------------
        float getSampleAt(int index)
        {
            return m_delay_line[0]->getSampleAt(index);
        }

        //----------------------------------------------------------------------------
        float getSampleDelayedBy(int delay)
        {
            return m_delay_line[0]->getSampleDelayedBy(delay);
        }

        //----------------------------------------------------------------------------
        float process(float in)
        {
            float out = m_delay_line[0]->read();
            in = in + (!m_change_sign ? (-1.0f) : 1.0f) * out * m_feedback;
            common::add_dc(in);
            m_delay_line[0]->write(in);
            return out + (!m_change_sign ? 1.0f : (-1.0f)) * in * m_feedback;
        }

#if defined(OPS_GUI)
        //----------------------------------------------------------------------------
        void clear()
        {
            m_delay_line[0]->clear();
        }
#endif

private:
        vector<unique_ptr<Delay>> m_delay_line;
        float m_feedback;
        bool m_change_sign;
};

#endif
