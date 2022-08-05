#pragma once

#ifndef CHORUS_HPP
#define CHORUS_HPP
#if defined(WIN32)
#include <string.h>
#endif

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"
#include "verbatim_delay.hpp"
#include "verbatim_oscillator.hpp"

#define CHORUS_DELAY_RANGE_SECS 0.040f

#define OPS_USE_LINEAR_INTERPOLATION

#if defined(USE_VGL) && USE_VGL
#include "vgl/vgl_vector.hpp"
using vgl::vector;
#else
#include <vector>
using std::vector;
#include <memory>
using std::shared_ptr;
using std::unique_ptr;
#endif

class Chorus
{
    public:
        //----------------------------------------------------------------------------
        Chorus(void)
        {
            m_osc.emplace_back(new Oscillator());
            m_delay_lines.emplace_back(new Delay());
            m_delay_lines.emplace_back(new Delay());

            m_mix = 0.5f;
            m_min_delay_time_sec = 0.0f;
            m_max_delay_time_sec = 40.0f;
            m_delay = 0.0f;
            m_depth = 1.0f;
            m_feedback = 0.0f;
#if defined(OPS_GUI)
            setSamplerate(44100.0f);
#endif
            m_osc[0]->setOscMode(k_oscmode_lfo);
            m_osc[0]->setWaveform(eOscWaveforms::sine);
            m_osc[0]->trigger(0.01f);

            m_delay_lines[0]->setDelayTime(g_delay_buffer_size);
            m_delay_lines[1]->setDelayTime(g_delay_buffer_size);
        }

        //----------------------------------------------------------------------------
        ~Chorus(void)
        {
        }

        //----------------------------------------------------------------------------
        void init(int params[], int num_params)
        {
            // setting all effect parameters as defined in the input array
            for (int ii = 0; ii < num_params; ++ii)
            {
                setParameter(ii, params[ii]);
            }
        }

        //----------------------------------------------------------------------------
        void setParameter(int parameter, int value)
        {
            setParameter(parameter, static_cast<float>(value) / 65535.0f);
        }

        //----------------------------------------------------------------------------
        void setParameter(int parameter, float value)
        {
            value = common::cclampf(value, 0.0f, 1.0f);
            switch (parameter)
            {
            case eChorus::k_chorus_delay:
                setDelay(value);
                break;

            case eChorus::k_chorus_mix:
                m_mix = value;
                break;

            case eChorus::k_chorus_rate:
                setRate(value);
                break;

            case eChorus::k_chorus_feedback:
                setFeedback(value);
                break;

            case eChorus::k_chorus_depth:
                setDepth(value);
                break;

            default:
                break;
            }
        }

        //----------------------------------------------------------------------------
        void setDelay(float value)
        {
#if defined(USE_LD)
            assert(value >= 0.0f && value <= 1.0f);
#endif
            m_delay = value;
            setDelayRange();
        }

        //----------------------------------------------------------------------------
        void setDepth(float depth)
        {
#if defined(USE_LD)
            assert(depth >= 0.0f && depth <= 1.0f);
#endif
            m_depth = depth;
            setDelayRange();
        }

        //----------------------------------------------------------------------------
        void setDelayRange()
        {
#if defined(USE_LD)
            assert(m_delay >= 0.0f && m_delay <= 1.0f);
            assert(m_depth >= 0.0f && m_depth <= 1.0f);
#endif
            m_min_delay_time_sec = m_delay * CHORUS_DELAY_RANGE_SECS;
            m_max_delay_time_sec = m_min_delay_time_sec + (m_depth * CHORUS_DELAY_RANGE_SECS);
        }

        //----------------------------------------------------------------------------
        void setFeedback(float feedback)
        {
            m_feedback = feedback;
        }

        //----------------------------------------------------------------------------
        void setRate(float rate)
        {
            m_osc[0]->setPitch(rate);
        }

        //----------------------------------------------------------------------------
#if defined(OPS_GUI)
        void setSamplerate(float samplerate)
        {
            m_samplerate = samplerate;
        }

        void clear()
        {
            m_delay_lines[0]->clear();
            m_delay_lines[1]->clear();
        }
#endif

        //----------------------------------------------------------------------------
        void process(float *inputs, float *outputs)
        {
            float center_delay_time_sec = (m_max_delay_time_sec + m_min_delay_time_sec) * 0.5f;
#if defined(USE_LD)
            assert(center_delay_time_sec >= 0.0f);
#endif

#if defined(OPS_USE_LINEAR_INTERPOLATION)
            float sample_offset = M_SAMPLERATE * (center_delay_time_sec + (m_osc[0]->getSample() * m_depth * center_delay_time_sec));
#else
            int sample_offset = common::clrintf(M_SAMPLERATE * (center_delay_time_sec + (osc_val * m_depth * center_delay_time_sec)));
#endif
            float dry[2];
#if defined(USE_LD)
            assert(m_mix >= 0.0f && m_mix <= 1.0f);
#endif
            for (int ii = 0; ii < 2; ++ii)
            {
                dry[ii] = inputs[ii];
                outputs[ii] = m_delay_lines[ii]->getSampleDelayedBy(sample_offset);
                inputs[ii] += (m_feedback * outputs[ii]);
                common::add_dc(inputs[ii]);
                m_delay_lines[ii]->write(inputs[ii]);
                outputs[ii] = ((1.0f - m_mix) * dry[ii]) + (m_mix * outputs[ii]);
            }
        }

    private:
#if defined(OPS_GUI)
        float m_samplerate;
#endif
        float m_mix;
#if defined(OPS_GUI)
        vector<shared_ptr<Delay>> m_delay_lines;
#else
        vector<unique_ptr<Delay>> m_delay_lines;
#endif
        vector<unique_ptr<Oscillator>> m_osc;

        float m_delay;
        float m_depth;
        float m_feedback;
        float m_min_delay_time_sec;
        float m_max_delay_time_sec;
};

#endif
