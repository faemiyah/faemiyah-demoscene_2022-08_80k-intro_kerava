#pragma once

#ifndef OPS_STEREO_FILTER_HPP
#define OPS_STEREO_FILTER_HPP

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"
#include "verbatim_filter.hpp"

//----------------------------------------------------------------------------
// Stereo filter
//----------------------------------------------------------------------------

class StereoFilter
{
public:
    //----------------------------------------------------------------------------
    explicit StereoFilter(void)
    {
#if defined(OPS_GUI)
        setSamplerate(44100.0f);
#endif
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
        case eStereoFilter::k_filter_mode:
            m_filters[0].setMode(value);
            m_filters[1].setMode(value);
            break;

        case eStereoFilter::k_filter_cutoff:
            m_filters[0].setCutoff(value);
            m_filters[1].setCutoff(value);
            break;

        case eStereoFilter::k_filter_resonance:
            m_filters[0].setResonance(value);
            m_filters[1].setResonance(value);
            break;

        default:
            break;
        }
    }

#if defined(OPS_GUI)
    //----------------------------------------------------------------------------
    void setSamplerate(float samplerate)
    {
        assert(samplerate > 0.0f);
        m_filters[0].setSamplerate(samplerate);
        m_filters[1].setSamplerate(samplerate);
    }
#endif

    //----------------------------------------------------------------------------
    void process(float *inputs, float *outputs)
    {
        outputs[0] = m_filters[0].process(inputs[0]);
        outputs[1] = m_filters[1].process(inputs[1]);
    }

private:
    array<Filter, 2u> m_filters;
};

#endif
