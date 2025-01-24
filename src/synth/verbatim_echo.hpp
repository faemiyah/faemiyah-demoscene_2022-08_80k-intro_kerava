#pragma once

#ifndef OPS_ECHO_HPP
#define OPS_ECHO_HPP

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"
#include "verbatim_delay.hpp"
#include "verbatim_onepole_filter.hpp"

//----------------------------------------------------------------------------
// Echo
//----------------------------------------------------------------------------

class Echo
{
public:
    //----------------------------------------------------------------------------
    Echo(void)
    {
#if defined(OPS_GUI)
        setSamplerate(44100.0f);
#endif
        construct();
    }

    void construct()
    {
        m_mode = eDelayModes::off;
        m_mix = 0.5f;
        m_feedback = 0.0f;

        for (int ii = 0; ii < 2; ++ii)
        {
            m_delay_lines.emplace_back(new Delay());
            m_lowpass_filters.emplace_back(new OnePoleLPF());
            m_highpass_filters.emplace_back(new OnePoleLPF());
        }

        for (int ii = 0; ii < 2; ++ii)
        {
            m_lowpass_filters[ii]->setGain(1.0f);
            m_highpass_filters[ii]->setGain(0.0f);
        }
    }

    //----------------------------------------------------------------------------
    ~Echo(void)
    {
    }

    //----------------------------------------------------------------------------
    void init(int params[], int num_params)
    {
        // Setting all instrument parameters as defined in the input array
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
        case eEcho::k_delay_mode:
            setMode(value);
            break;

        case eEcho::k_delay_mix:
            m_mix = value;
            break;

        case eEcho::k_delay_time:
            m_delay_lines[0]->setDelayTime(value);
            m_delay_lines[1]->setDelayTime(value);
            break;

        case eEcho::k_delay_feedback:
            m_feedback = value;
            m_delay_lines[0]->setFeedback(value);
            m_delay_lines[1]->setFeedback(value);
            break;

        case eEcho::k_delay_lowpass_cutoff:
            setLowpassCutoff(value);
            break;

        case eEcho::k_delay_highpass_cutoff:
            setHighpassCutoff(value);
            break;

        default:
            break;
        }
    }

    //----------------------------------------------------------------------------
    void setMode(float value)
    {
#if defined(DNLOAD_USE_LD)
        assert(value >= 0.0f && value <= 1.0f);
#endif
        setMode(common::clrintf(value * static_cast<float>(eDelayModes::num_items - 1)));
    }

    //----------------------------------------------------------------------------
    void setMode(int value)
    {
#if defined(DNLOAD_USE_LD)
        assert(value >= 0 && value < eDelayModes::num_items);
        if (value >= eDelayModes::num_items)
        {
            std::ostringstream sstr;
            sstr << "Echo::setMode value " << value << " out of bounds. Should be less than " << eDelayModes::num_items << std::endl;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        m_mode = value;
    }

    //----------------------------------------------------------------------------
    void setLowpassCutoff(float lowpass_cutoff)
    {
        m_lowpass_filters[0]->setGain(lowpass_cutoff);
        m_lowpass_filters[1]->setGain(lowpass_cutoff);
    }

    //----------------------------------------------------------------------------
    void setHighpassCutoff(float highpass_cutoff)
    {
        m_highpass_filters[0]->setGain(highpass_cutoff);
        m_highpass_filters[1]->setGain(highpass_cutoff);
    };

#if defined(OPS_GUI)
    //----------------------------------------------------------------------------
    void setSamplerate(float samplerate)
    {
#if defined(DNLOAD_USE_LD)
        assert(samplerate > 0.0f);
#endif
        m_samplerate = samplerate;
    }

    //----------------------------------------------------------------------------
    void clear()
    {
        m_delay_lines[0]->clear();
        m_delay_lines[1]->clear();
    }
#endif

    //----------------------------------------------------------------------------
    void process(float *inputs, float *outputs)
    {
        int ii = 0;
        float temp[2] = { inputs[0], inputs[1] };
        switch (m_mode)
        {
        case eDelayModes::mono:
            outputs[0] = m_delay_lines[0]->process(0.5f * (temp[0] + temp[1]));
            outputs[1] = m_delay_lines[1]->process(0.5f * (temp[0] + temp[1]));
            break;

        case eDelayModes::stereo:
            outputs[0] = m_delay_lines[0]->process(temp[0]);
            outputs[1] = m_delay_lines[1]->process(temp[1]);
            break;

        case eDelayModes::pingpong:
            temp[0] = 0.5f * (temp[0] + temp[1]);
            temp[1] = 0.0f;
            // Fallthrough to cross delay is intentional here.
            /* FALLTHRU */

        case eDelayModes::cross:
#if defined(DNLOAD_USE_LD)
            assert(m_feedback >= 0.0f && m_feedback <= 1.0f);
#endif
            for (ii = 0; ii < 2; ++ii)
            {
                outputs[ii] = m_delay_lines[1 - ii]->read();
                temp[ii] += (m_feedback * outputs[ii]);
                temp[ii] = m_lowpass_filters[ii]->process(temp[ii]);
                temp[ii] -= m_highpass_filters[ii]->process(temp[ii]);
                common::add_dc(temp[ii]);
                m_delay_lines[ii]->write(temp[ii]);
            }
            break;

        case eDelayModes::off:
        default:
            m_delay_lines[0]->process(0.0f);
            m_delay_lines[1]->process(0.0f);
            break;
        }

#if defined(DNLOAD_USE_LD)
        assert(m_mix >= 0.0f && m_mix <= 1.0f);
#endif
        outputs[0] = ((1.0f - m_mix) * inputs[0]) + (m_mix * outputs[0]);
        outputs[1] = ((1.0f - m_mix) * inputs[1]) + (m_mix * outputs[1]);
    }

private:
#if defined(OPS_GUI)
    float m_samplerate;
#endif
    float m_mix;
    vector<unique_ptr<Delay>> m_delay_lines;
    int m_mode;
    float m_feedback;
    vector<unique_ptr<OnePoleLPF>> m_lowpass_filters;
    vector<unique_ptr<OnePoleLPF>> m_highpass_filters;
};

#endif
