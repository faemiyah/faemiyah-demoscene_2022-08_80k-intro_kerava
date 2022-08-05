#pragma once

#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#define OPS_FOLDBACK_COEFF 10.0f
#define OPS_WRAP_COEFF 10.0f
#define OPS_CLIP_COEFF 10.0f
#define OPS_TANH_COEFF 30.0f
#define OPS_POST_GAIN_COEFF 10.0f

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"

#if defined(OPS_GUI)
#define OPS_OVERSAMPLING_FACTOR 8
// log2 of oversampling factor
#define OPS_OVERSAMPLING_STAGES 3
// 2 filters per stage (up and down), stereo
#define OPS_NUM_HBFS 2 * 2 * OPS_OVERSAMPLING_STAGES
#include "BandLimit.hpp"
#else
//#include "synth/BandLimit.hpp"
#endif
#include "ops_log.hpp"

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

//----------------------------------------------------------------------------
// Distortion
//----------------------------------------------------------------------------

class DistortionProcessor
{
public:
    //----------------------------------------------------------------------------
    DistortionProcessor(void)
    {
        m_sh_counter = 0;
        m_sh_period = 0;
    }

    //----------------------------------------------------------------------------
    ~DistortionProcessor(void)
    {
    }

    float distort(float in, int mode, float drive)
    {
        float bits;
        float bit_depth_multi;
        float out = 0.0f;

        switch (mode)
        {
        case eDistortionModes::tanh:
            out = ops_tanhf(in * (1.0f + (OPS_TANH_COEFF * drive)));
            break;

        case eDistortionModes::clip:
            out = drive * OPS_CLIP_COEFF * in;
            out = common::cclamp1f(out);
            break;

        case eDistortionModes::wrap:
            // naive, TODO: find a more efficient solution
            out = drive * OPS_WRAP_COEFF * in;
            while (out < -1.0f) { out += 2.0f; }
            while (out > 1.0f) { out -= 2.0f; }
            break;

        case eDistortionModes::foldback:
            out = ops_sinf(in * OPS_FOLDBACK_COEFF * drive);
            break;

        case eDistortionModes::bit_depth:
            bits = (1.0f - drive) * 16.0f;
            bit_depth_multi = ops_exp2f(bits);
            out = static_cast<float>(static_cast<int>(in * bit_depth_multi)) / bit_depth_multi;
            break;

        case eDistortionModes::sample_rate:
            --m_sh_counter;
            // 440 used as an arbitrary period. TODO: calculate a sensible value
            m_sh_period = static_cast<int>(drive * 440.0f);
            if (m_sh_counter <= 0)
            {
                out = in;
                m_sh_counter = m_sh_period;
            }
            break;

        case eDistortionModes::pass_through:
            out = in;
            break;

        case eDistortionModes::off:
        default:
            break;
        }

        return out;
    }

    int m_sh_counter;
    int m_sh_period;
};

class Distortion
{
public:
    //----------------------------------------------------------------------------
    Distortion(void)
    {
        m_mode = eDistortionModes::off;
        m_mix = 0.5f;
        m_drive = 0.0f;
        m_post_gain = 0.5f;
        m_distproc.emplace_back(new DistortionProcessor()); // L
        m_distproc.emplace_back(new DistortionProcessor()); // R
#if defined(OPS_GUI)
        setSamplerate(44100.0f);
        m_oversampling_mode = eOversamplingModes::fixed_x2;
#endif

#if defined(OPS_GUI)
        // Initialize half band filters for up and downsampling, 2 channels, 2 filters per factor
        // (first for up, then downsampling)
        m_hbfs = new CHalfBandFilter*[OPS_NUM_HBFS];
        for (int ii = 0; ii < OPS_NUM_HBFS; ++ii)
        {
            m_hbfs[ii] = new CHalfBandFilter(8, false);
        }
#endif
        clear();
    }

    //----------------------------------------------------------------------------
    ~Distortion(void)
    {
#if defined(OPS_GUI)
        for (int ii = OPS_NUM_HBFS - 1;	ii > 0; --ii)
        {
            delete m_hbfs[ii];
        }
        delete[] m_hbfs;
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
        case eDist::k_dist_mode:
            setMode(value);
            break;

        case eDist::k_dist_mix:
            m_mix = value;
            break;

        case eDist::k_dist_drive:
            m_drive = value;
            break;

        case eDist::k_dist_post_gain:
            m_post_gain = value;
            break;

        case eDist::k_oversampling_mode:
            m_oversampling_mode = common::clrintf(value * static_cast<float>(eOversamplingModes::num_items - 1));
            switch (m_oversampling_mode)
            {
            case eOversamplingModes::x2:
                m_oversampling_stages = 1;
                break;

            case eOversamplingModes::x4:
                m_oversampling_stages = 2;
                break;

            case eOversamplingModes::x8:
                m_oversampling_stages = 3;
                break;

            default:
                m_oversampling_stages = 0;
                break;
            }
            clear();
            break;

        default:
            break;
        }
    }

    //----------------------------------------------------------------------------
    void setMode(float value)
    {
#if defined(USE_LD)
        assert(value >= 0.0f && value <= 1.0f);
#endif
        setMode(common::clrintf(value * static_cast<float>(eDistortionModes::num_items - 1)));
    }

    //----------------------------------------------------------------------------
    void setMode(int value)
    {
#if defined(USE_LD)
        assert(value >= 0 && value < eDistortionModes::num_items);
        if (value >= eDistortionModes::num_items)
        {
            std::ostringstream sstr;
            sstr << "Echo::setMode value " << value << " out of bounds. Should be less than " << eDelayModes::num_items << std::endl;
            BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        m_mode = value;
        clear();
    }

#if defined(OPS_GUI)
    //----------------------------------------------------------------------------
    void setSamplerate(float samplerate)
    {
#if defined(USE_LD)
        assert(samplerate > 0.0f);
#endif
        m_samplerate = samplerate;
    }
#endif

    //----------------------------------------------------------------------------
    void clear()
    {
        maxval = 0.0f;
#if defined(OPS_GUI)
        ops_memset(m_temp, 0, 2 * OPS_OVERSAMPLING_FACTOR * sizeof(float));
        for (int ii = 0; ii < OPS_NUM_HBFS; ++ii)
        {
            m_hbfs[ii]->clear();
        }
#endif
    }

    //----------------------------------------------------------------------------
    void process(float *inputs, float *outputs)
    {
#if defined(OPS_GUI)
        int stage = 0;

        switch (m_oversampling_mode)
        {
        case eOversamplingModes::x2:
        case eOversamplingModes::x4:
        case eOversamplingModes::x8:

            // Seed the "first" samples for the oversampling buffers for both channels
            // and leave the samples between empty as they will get filled in the
            // following passes
            m_temp[0] = inputs[0];

            m_temp[OPS_OVERSAMPLING_FACTOR] = inputs[1];

            for (stage = 0; stage < m_oversampling_stages; ++stage)
            {
                for (int ii = 0; ii < (2 << stage); ii += 2)
                {
                    m_temp[ii] = m_hbfs[stage]->process(m_temp[ii / 2]) * 2.0f;
                    m_temp[ii + 1] = m_hbfs[stage]->process(0.0f) * 2.0f; // zero fill every other sample
                }
            }

            for (int ii = 0; ii < 2 * OPS_OVERSAMPLING_FACTOR; ii+=2)
            {
                m_temp[ii] = m_distproc[0]->distort(m_temp[ii], m_mode, m_drive);
                m_temp[OPS_OVERSAMPLING_FACTOR + ii] = m_distproc[0]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + ii], m_mode, m_drive);
            }

            for (stage = (m_oversampling_stages - 1); stage >= 0; --stage)
            {
                for (int ii = 0; ii < (2 << stage); ii += 2)
                {
                    // drop every other sample when downsampling
                    m_temp[(ii / 2)] = m_hbfs[OPS_OVERSAMPLING_STAGES + stage]->process(m_temp[ii]);
                    m_temp[(ii / 2) + 1] = m_hbfs[OPS_OVERSAMPLING_STAGES + stage]->process(m_temp[ii + 1]);
                }
            }
            break;

        case eOversamplingModes::fixed_x2:
            // zero fill every other sample
            m_temp[0] = m_hbfs[0]->process(inputs[0]) * 2.0f;
            m_temp[1] = m_hbfs[0]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR] = m_hbfs[2 * OPS_OVERSAMPLING_STAGES]->process(inputs[1]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_hbfs[2 * OPS_OVERSAMPLING_STAGES]->process(0.0f) * 2.0f;

            m_temp[0] = m_distproc[0]->distort(m_temp[0], m_mode, m_drive);
            m_temp[1] = m_distproc[0]->distort(m_temp[1], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 1], m_mode, m_drive);

            // drop every other sample after filtering
            m_temp[0] = m_hbfs[1]->process(m_temp[0]);
            m_temp[1] = m_hbfs[1]->process(m_temp[1]);
            m_temp[8] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR]);
            m_temp[9] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 1]);
            break;

        case eOversamplingModes::fixed_x4:
            // zero fill every other sample
            m_temp[0] = m_hbfs[0]->process(inputs[0]) * 2.0f;
            m_temp[1] = m_hbfs[0]->process(0.0f) * 2.0f;
            m_temp[0] = m_hbfs[1]->process(m_temp[0]) * 2.0f;
            m_temp[1] = m_hbfs[1]->process(0.0f) * 2.0f;
            m_temp[2] = m_hbfs[1]->process(m_temp[1]) * 2.0f;
            m_temp[3] = m_hbfs[1]->process(0.0f) * 2.0f;

            m_temp[OPS_OVERSAMPLING_FACTOR] = m_hbfs[2 * OPS_OVERSAMPLING_STAGES]->process(inputs[1]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_hbfs[2 * OPS_OVERSAMPLING_STAGES]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 2] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 1]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 3] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(0.0f) * 2.0f;

            m_temp[0] = m_distproc[0]->distort(m_temp[0], m_mode, m_drive);
            m_temp[1] = m_distproc[0]->distort(m_temp[1], m_mode, m_drive);
            m_temp[2] = m_distproc[0]->distort(m_temp[2], m_mode, m_drive);
            m_temp[3] = m_distproc[0]->distort(m_temp[3], m_mode, m_drive);

            m_temp[OPS_OVERSAMPLING_FACTOR] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 1], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 2] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 2], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 3] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 3], m_mode, m_drive);

            // drop every other sample after filtering
            m_temp[0] = m_hbfs[5]->process(m_temp[0]);
            m_temp[1] = m_hbfs[5]->process(m_temp[1]);
            m_temp[2] = m_hbfs[5]->process(m_temp[2]);
            m_temp[3] = m_hbfs[5]->process(m_temp[3]);
            m_temp[0] = m_hbfs[4]->process(m_temp[0]);
            m_temp[1] = m_hbfs[4]->process(m_temp[2]);

            m_temp[8] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR]);
            m_temp[9] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 1]);
            m_temp[10] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 2]);
            m_temp[11] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 3]);
            m_temp[8] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES)]->process(m_temp[OPS_OVERSAMPLING_FACTOR]);
            m_temp[9] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES)]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 2]);
            break;

        case eOversamplingModes::fixed_x8:
            // zero fill every other sample
            m_temp[0] = m_hbfs[0]->process(inputs[0]) * 2.0f;
            m_temp[1] = m_hbfs[0]->process(0.0f) * 2.0f;
            m_temp[0] = m_hbfs[1]->process(m_temp[0]) * 2.0f;
            m_temp[1] = m_hbfs[1]->process(0.0f) * 2.0f;
            m_temp[2] = m_hbfs[1]->process(m_temp[1]) * 2.0f;
            m_temp[3] = m_hbfs[1]->process(0.0f) * 2.0f;
            m_temp[0] = m_hbfs[2]->process(m_temp[0]) * 2.0f;
            m_temp[1] = m_hbfs[2]->process(0.0f) * 2.0f;
            m_temp[2] = m_hbfs[2]->process(m_temp[1]) * 2.0f;
            m_temp[3] = m_hbfs[2]->process(0.0f) * 2.0f;
            m_temp[4] = m_hbfs[2]->process(m_temp[2]) * 2.0f;
            m_temp[5] = m_hbfs[2]->process(0.0f) * 2.0f;
            m_temp[6] = m_hbfs[2]->process(m_temp[3]) * 2.0f;
            m_temp[7] = m_hbfs[2]->process(0.0f) * 2.0f;

            m_temp[OPS_OVERSAMPLING_FACTOR] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES)]->process(inputs[1]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES)]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 2] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 1]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 3] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 1]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 2] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 1]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 3] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 4] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 2]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 5] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(0.0f) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 6] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 3]) * 2.0f;
            m_temp[OPS_OVERSAMPLING_FACTOR + 7] = m_hbfs[(2 * OPS_OVERSAMPLING_STAGES) + 2]->process(0.0f) * 2.0f;

            m_temp[0] = m_distproc[0]->distort(m_temp[0], m_mode, m_drive);
            m_temp[1] = m_distproc[0]->distort(m_temp[1], m_mode, m_drive);
            m_temp[2] = m_distproc[0]->distort(m_temp[2], m_mode, m_drive);
            m_temp[3] = m_distproc[0]->distort(m_temp[3], m_mode, m_drive);
            m_temp[4] = m_distproc[0]->distort(m_temp[4], m_mode, m_drive);
            m_temp[5] = m_distproc[0]->distort(m_temp[5], m_mode, m_drive);
            m_temp[6] = m_distproc[0]->distort(m_temp[6], m_mode, m_drive);
            m_temp[7] = m_distproc[0]->distort(m_temp[7], m_mode, m_drive);

            m_temp[OPS_OVERSAMPLING_FACTOR] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 1] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 1], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 2] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 2], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 3] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 3], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 4] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 4], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 5] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 5], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 6] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 6], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR + 7] = m_distproc[1]->distort(m_temp[OPS_OVERSAMPLING_FACTOR + 7], m_mode, m_drive);

            // drop every other sample after filtering
            m_temp[0] = m_hbfs[5]->process(m_temp[0]);
            m_temp[1] = m_hbfs[5]->process(m_temp[1]);
            m_temp[2] = m_hbfs[5]->process(m_temp[2]);
            m_temp[3] = m_hbfs[5]->process(m_temp[3]);
            m_temp[4] = m_hbfs[5]->process(m_temp[4]);
            m_temp[5] = m_hbfs[5]->process(m_temp[5]);
            m_temp[6] = m_hbfs[5]->process(m_temp[6]);
            m_temp[7] = m_hbfs[5]->process(m_temp[7]);
            m_temp[0] = m_hbfs[4]->process(m_temp[0]);
            m_temp[1] = m_hbfs[4]->process(m_temp[2]);
            m_temp[2] = m_hbfs[4]->process(m_temp[4]);
            m_temp[3] = m_hbfs[4]->process(m_temp[6]);
            m_temp[0] = m_hbfs[3]->process(m_temp[0]);
            m_temp[1] = m_hbfs[3]->process(m_temp[2]);

            m_temp[8] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR]);
            m_temp[9] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 1]);
            m_temp[10] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 2]);
            m_temp[11] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 3]);
            m_temp[12] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 4]);
            m_temp[13] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 5]);
            m_temp[14] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 6]);
            m_temp[15] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 2]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 7]);
            m_temp[8] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR]);
            m_temp[9] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 2]);
            m_temp[10] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 4]);
            m_temp[11] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES) + 1]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 6]);
            m_temp[8] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES)]->process(m_temp[OPS_OVERSAMPLING_FACTOR]);
            m_temp[9] = m_hbfs[(3 * OPS_OVERSAMPLING_STAGES)]->process(m_temp[OPS_OVERSAMPLING_FACTOR + 2]);
            break;

        default:
            m_temp[0] = m_distproc[0]->distort(inputs[0], m_mode, m_drive);
            m_temp[OPS_OVERSAMPLING_FACTOR] = m_distproc[1]->distort(inputs[1], m_mode, m_drive);
            break;
        }
        assert(m_mix >= 0.0f && m_mix <= 1.0f);
        outputs[0] = m_post_gain * OPS_POST_GAIN_COEFF * (((1.0f - m_mix) * inputs[0]) + (m_mix * m_temp[0]));
        outputs[1] = m_post_gain * OPS_POST_GAIN_COEFF * (((1.0f - m_mix) * inputs[1]) + (m_mix * m_temp[OPS_OVERSAMPLING_FACTOR]));
#else
        float m_temp_l = m_distproc[0]->distort(inputs[0], m_mode, m_drive);
        float m_temp_r = m_distproc[1]->distort(inputs[1], m_mode, m_drive);
        outputs[0] = m_post_gain * OPS_POST_GAIN_COEFF * (((1.0f - m_mix) * inputs[0]) + (m_mix * m_temp_l));
        outputs[1] = m_post_gain * OPS_POST_GAIN_COEFF * (((1.0f - m_mix) * inputs[1]) + (m_mix * m_temp_r));
#endif
    }

private:
#if defined(OPS_GUI)
    float m_samplerate;
    float m_temp[2 * OPS_OVERSAMPLING_FACTOR];
    CHalfBandFilter** m_hbfs;
#endif
    float m_mix;
    int m_mode;
    float m_drive;
    float m_post_gain;
    // 2 samples for each oversampling stage, first half for left, second half for right channel
    // samples (Ls1 Ls2 Ls3 Ls4 Rs1 Rs2 Rs3 Rs4 etc.)
    int m_oversampling_mode;
    int m_oversampling_stages;
    float maxval;

    vector<unique_ptr<DistortionProcessor>> m_distproc;
};
#endif
