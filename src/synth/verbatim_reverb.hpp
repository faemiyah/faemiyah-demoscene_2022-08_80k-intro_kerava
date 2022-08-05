#pragma once

#ifndef REVERB_HPP
#define REVERB_HPP

#include "verbatim_parameters.hpp"
#include "verbatim_common.hpp"
#include "verbatim_delay.hpp"
//#include "verbatim_onepole_filter.hpp"
#include "verbatim_filter.hpp"

// TODO: implement apf modulation
//#include "oscillator.hpp"

/*
Dattorro plate reverb gain and decay values (from Will Pirkle's Designing Audio Effect Plug-Ins in C++, table 11.2)
values are for 44.1k samplerates

APF dx / delay(samples) / g(index) / g(value)
1,  210,  1, 0.75
2,  158,  1, 0.75
3,  561,  2, 0.625
4,  410,  2, 0.625
9,  3931, 3, 0.5
10, 2664, 3, 0.5

Fixed Delay dx / delay(samples)
7,  6241
8,  6590
11, 4641 (possibly 4681?)
12, 5505

Mod APF dx / delay(samples) / g(index) / g(value)
5, 1343+-12, 4, 0.7
6, 995 +-12, 4, 0.7
*/

#if defined(RVB_44100)
#define PREDELAY_LEN (221.0f / 44100.0f)

#define APF1_LEN (210.0f / 44100.0f)
#define APF2_LEN (158.0f / 44100.0f)
#define APF3_LEN (561.0f / 44100.0f)
#define APF4_LEN (410.0f / 44100.0f)
#define APF6_LEN (3931.0f / 44100.0f)
#define APF5_LEN (2664.0f / 44100.0f)

#define DELAY1_LEN (6241.0f / 44100.0f)
#define DELAY2_LEN (6590.0f / 44100.0f)
#define DELAY3_LEN (4641.0f / 44100.0f)
#define DELAY4_LEN (5505.0f / 44100.0f)

#define MODAPF2_LEN (1343.0f / 44100.0f)
#define MODAPF1_LEN (995.0f / 44100.0f)
#else
#define PREDELAY_LEN (221.0f / 29761.0f)

// input diffusion 1
#define APF1_LEN (142.0f / 29761.0f)
// input diffusion 1
#define APF2_LEN (107.0f / 29761.0f)
// input diffusion 2
#define APF3_LEN (379.0f / 29761.0f)
// input diffusion 2
#define APF4_LEN (277.0f / 29761.0f)

// decay diffusion 1 (node 23) (should have mod +-12)
#define MODAPF1_LEN (672.0f / 29761.0f)
// decay diffusion 1    (node 46) (should have mod +-12)
#define MODAPF2_LEN (908.0f / 29761.0f)

// decay diffusion 2 (node 31)
#define APF5_LEN (1800.0f / 29761.0f)
// decay diffusion 2 (node 55)
#define APF6_LEN (2656.0f / 29761.0f)

// delay (node 30)
#define DELAY1_LEN (4453.0f / 29761.0f)
// delay (node 39)
#define DELAY2_LEN (3720.0f / 29761.0f)
// delay (node 54)
#define DELAY3_LEN (4217.0f / 29761.0f)
// delay (node 63)
#define DELAY4_LEN (3163.0f / 29761.0f)

// From Jon Dattorro's Effect Design Part 1: Reverberator and Other Filters, Table 2
/*********    left output,    all wet ********
accumulator = 0.6 X node48_54[266]
accumulator += 0.6 x node48_54[2974]
accumulator -= 0.6 X node55_59[1913]
accumulator += 0.6 X node59_63[1996]
accumulator -= 0.6 X node24_30[1990]
accumulator -= 0.6 x node31_33[187]
YL = accumulator - 0.6 X node33_39[1066]
*/
#define LEFT_TAP1_DELAY3 (266.0f / 29761.0f)
#define LEFT_TAP2_DELAY3 (2974.0f / 29761.0f)
#define LEFT_TAP3_APF6 (1913.0f / 29761.0f)
#define LEFT_TAP4_DELAY4 (1996.0f / 29761.0f)
#define LEFT_TAP5_DELAY1 (1990.0f / 29761.0f)
#define LEFT_TAP6_APF5 (187.0f / 29761.0f)
#define LEFT_TAP7_DELAY2 (1066.0f / 29761.0f)

/*********    right output, all wet ********
accumulator = 0.6 X node24_30[353]
accumulator += 0.6 X node24_30[3627]
accumulator -= 0.6 X node31_33[1228]
accumulator += 0.6 X node33_39[2673]
accumulator -= 0.6 X node48_54[2111]
accumulator -= 0.6 X node55_59[335]
YR = accumulator - 0.6 X node59_63[121]
*/
#define RIGHT_TAP1_DELAY1 (353.0f / 29761.0f)
#define RIGHT_TAP2_DELAY1 (3627.0f / 29761.0f)
#define RIGHT_TAP3_APF5 (1228.0f / 29761.0f)
#define RIGHT_TAP4_DELAY2 (2673.0f / 29761.0f)
#define RIGHT_TAP5_DELAY3 (2111.0f / 29761.0f)
#define RIGHT_TAP6_APF6 (335.0f / 29761.0f)
#define RIGHT_TAP7_DELAY4 (121.0f / 29761.0f)

#endif

class Reverb
{
    enum reverb_indices
    {
        // Allpass filters
        rvb_apf1 = 0,
        rvb_apf2 = 1,
        rvb_apf3 = 2,
        rvb_apf4 = 3,
        rvb_apf5 = 4,
        rvb_apf6 = 5,
        // Modulated allpass filters
        rvb_mod_apf1 = 6,
        rvb_mod_apf2 = 7,
        // Static delays
        rvb_predelay = 0,
        rvb_delay1 = 1,
        rvb_delay2 = 2,
        rvb_delay3 = 3,
        rvb_delay4 = 4,
        // Filters for bandwidth and damping
        rvb_filter_bandwidth = 0,
        rvb_filter_damping1 = 1,
        rvb_filter_damping2 = 2
    };

#define NUM_RVB_DELAYS 5
#define NUM_RVB_APFS 8
#define NUM_RVB_FILTERS 3
    public:
        //----------------------------------------------------------------------------
        Reverb(void)
        {
#if defined(OPS_GUI)
            m_samplerate = 44100.0f;
#endif
            construct();
        }
        void construct()
        {
            for (int ii = 0; ii < NUM_RVB_DELAYS; ++ii)
            {
                m_delays.emplace_back(new Delay());
            }
            for (int ii = 0; ii < NUM_RVB_APFS; ++ii)
            {
                m_apfs.emplace_back(new AllPass());
            }
            for (int ii = 0; ii < NUM_RVB_FILTERS; ++ii)
            {
                m_filters.emplace_back(new Filter());
                m_filters[ii]->setMode(eFilterModes::lowpass);
            }

            // control params
            m_predelay_len = (44100.0f / 441.0f);
            m_filters[rvb_filter_bandwidth]->setCutoff(0.9995f);
            m_decay = 0.5f;
            setDamping(0.0005f);
            m_roomsize = 0.5f;
            setMix(0.5f);

            m_g1 = 0.75f;
            m_g2 = 0.625f;
            m_g3 = 0.5f;
            m_g4 = 0.7f;
            m_g5 = m_decay;

            m_apfs[rvb_apf1]->setDelayTime(common::clrintf(APF1_LEN * M_SAMPLERATE));
            m_apfs[rvb_apf2]->setDelayTime(common::clrintf(APF2_LEN * M_SAMPLERATE));
            m_apfs[rvb_apf3]->setDelayTime(common::clrintf(APF3_LEN * M_SAMPLERATE));
            m_apfs[rvb_apf4]->setDelayTime(common::clrintf(APF4_LEN * M_SAMPLERATE));
            m_apfs[rvb_apf5]->setDelayTime(common::clrintf(APF5_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_apf6]->setDelayTime(common::clrintf(APF6_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_apf1]->setFeedback(m_g1);
            m_apfs[rvb_apf2]->setFeedback(m_g1);
            m_apfs[rvb_apf3]->setFeedback(m_g2);
            m_apfs[rvb_apf4]->setFeedback(m_g2);
            m_apfs[rvb_apf5]->setFeedback(m_g4);
            m_apfs[rvb_apf6]->setFeedback(m_g4);

            m_delays[rvb_predelay]->setDelayTime(common::clrintf(PREDELAY_LEN * M_SAMPLERATE));
            m_delays[rvb_delay1]->setDelayTime(common::clrintf(DELAY1_LEN * M_SAMPLERATE * m_roomsize));
            m_delays[rvb_delay2]->setDelayTime(common::clrintf(DELAY2_LEN * M_SAMPLERATE * m_roomsize));
            m_delays[rvb_delay3]->setDelayTime(common::clrintf(DELAY3_LEN * M_SAMPLERATE * m_roomsize));
            m_delays[rvb_delay4]->setDelayTime(common::clrintf(DELAY4_LEN * M_SAMPLERATE * m_roomsize));

            m_apfs[rvb_mod_apf1]->setChangeSign(true);
            m_apfs[rvb_mod_apf2]->setChangeSign(true);
            m_apfs[rvb_mod_apf1]->setDelayTime(common::clrintf(MODAPF1_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_mod_apf2]->setDelayTime(common::clrintf(MODAPF2_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_mod_apf1]->setFeedback(m_g3);
            m_apfs[rvb_mod_apf2]->setFeedback(m_g3);

            m_feedback_left_tank = 0.0f;
            m_feedback_right_tank = 0.0f;
        }

        //----------------------------------------------------------------------------
        ~Reverb(void)
        {
        }

        //----------------------------------------------------------------------------
        // Set dry / wet mix ratio, 0.0f being completely dry, 1.0f completely wet
        void setMix(float mix) { m_mix_wet = mix; };

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
            case eReverb::k_reverb_mix:
                setMix(value);
                break;

            case eReverb::k_reverb_predelay:
                setPreDelayLength(value);
                break;

            case eReverb::k_reverb_bandwidth:
                setBandwidth(value);
                break;

            case eReverb::k_reverb_damping:
                setDamping(value);
                break;

            case eReverb::k_reverb_decay:
                setDecay(value);
                break;

            case eReverb::k_reverb_roomsize:
                setRoomSize(value);
                break;

            default:
                break;
            }
        }

        //----------------------------------------------------------------------------
        void setDecay(float decay)
        {
            m_decay = decay;
            m_g5 = m_decay;
        }

        //----------------------------------------------------------------------------
        void setPreDelayLength(float predelay_len)
        {
            m_predelay_len = predelay_len;
            m_delays[rvb_predelay]->setDelayTime(m_predelay_len);
        }

        //----------------------------------------------------------------------------
        void setDamping(float damping)
        {
            m_filters[rvb_filter_damping1]->setCutoff(1.0f - damping);
            m_filters[rvb_filter_damping2]->setCutoff(1.0f - damping);
        }

        //----------------------------------------------------------------------------
        void setRoomSize(float roomsize)
        {
            m_roomsize = roomsize;
            m_apfs[rvb_mod_apf1]->setDelayTime(common::clrintf(MODAPF1_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_mod_apf2]->setDelayTime(common::clrintf(MODAPF2_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_apf5]->setDelayTime(common::clrintf(APF5_LEN * M_SAMPLERATE * m_roomsize));
            m_apfs[rvb_apf6]->setDelayTime(common::clrintf(APF6_LEN * M_SAMPLERATE * m_roomsize));
        }

        //----------------------------------------------------------------------------
        void setBandwidth(float bandwidth)
        {
            m_filters[rvb_filter_bandwidth]->setCutoff(bandwidth);
        }

        //----------------------------------------------------------------------------
        void process(float *inputs, float *outputs)
        {
            // mixing to mono as dattorro's plate reverb design in mono in / stereo out
            float temp = (inputs[0] + inputs[1]) * 0.5f;
            common::add_dc(temp);
            // predelay and input diffusion stage
            temp = m_delays[rvb_predelay]->process(temp);
            temp = m_filters[rvb_filter_bandwidth]->process(temp);
            temp = m_apfs[rvb_apf1]->process(temp);
            temp = m_apfs[rvb_apf2]->process(temp);
            temp = m_apfs[rvb_apf3]->process(temp);
            temp = m_apfs[rvb_apf4]->process(temp);

            // feed input to both tanks
            m_feedback_right_tank += temp;
            m_feedback_left_tank += temp;

            // process left tank
            float left_tank = m_feedback_right_tank;
            left_tank = m_apfs[rvb_mod_apf1]->process(left_tank);
            left_tank = m_delays[rvb_delay1]->process(left_tank);
            left_tank = m_filters[rvb_filter_damping1]->process(left_tank);
            left_tank *= m_g5;
            common::add_dc(left_tank);
            left_tank = m_apfs[rvb_apf5]->process(left_tank);
            left_tank = m_delays[rvb_delay2]->process(left_tank);

            // process left tank
            float right_tank = m_feedback_left_tank;
            right_tank = m_apfs[rvb_mod_apf2]->process(right_tank);
            right_tank = m_delays[rvb_delay3]->process(right_tank);
            right_tank = m_filters[rvb_filter_damping2]->process(right_tank);
            right_tank *= m_g5;
            common::add_dc(right_tank);
            right_tank = m_apfs[rvb_apf6]->process(right_tank);
            right_tank = m_delays[rvb_delay4]->process(right_tank);

            // decay and set to be fed back to the other tank
            m_feedback_left_tank = left_tank * m_g5;
            m_feedback_right_tank = right_tank * m_g5;
            common::add_dc(m_feedback_left_tank);
            common::add_dc(m_feedback_right_tank);

            // combine outputs from taps
            float left_out = (0.6f * m_delays[rvb_delay3]->getSampleDelayedBy(common::clrintf(LEFT_TAP1_DELAY3 * M_SAMPLERATE)))
                + (0.6f * m_delays[rvb_delay3]->getSampleDelayedBy(common::clrintf(LEFT_TAP2_DELAY3 * M_SAMPLERATE)))
                - (0.6f * m_apfs[rvb_apf6]->getSampleDelayedBy(common::clrintf(LEFT_TAP3_APF6 * M_SAMPLERATE)))
                + (0.6f * m_delays[rvb_delay4]->getSampleDelayedBy(common::clrintf(LEFT_TAP4_DELAY4 * M_SAMPLERATE)))
                + (0.6f * m_delays[rvb_delay1]->getSampleDelayedBy(common::clrintf(LEFT_TAP5_DELAY1 * M_SAMPLERATE)))
                - (0.6f * m_apfs[rvb_apf5]->getSampleDelayedBy(common::clrintf(LEFT_TAP6_APF5 * M_SAMPLERATE)))
                - (0.6f * m_delays[rvb_delay2]->getSampleDelayedBy(common::clrintf(LEFT_TAP7_DELAY2 * M_SAMPLERATE)));

            float right_out = (0.6f * m_delays[rvb_delay1]->getSampleDelayedBy(common::clrintf(RIGHT_TAP1_DELAY1 * M_SAMPLERATE)))
                + (0.6f * m_delays[rvb_delay1]->getSampleDelayedBy(common::clrintf(RIGHT_TAP2_DELAY1 * M_SAMPLERATE)))
                - (0.6f * m_apfs[rvb_apf5]->getSampleDelayedBy(common::clrintf(RIGHT_TAP3_APF5 * M_SAMPLERATE)))
                + (0.6f * m_delays[rvb_delay2]->getSampleDelayedBy(common::clrintf(RIGHT_TAP4_DELAY2 * M_SAMPLERATE)))
                - (0.6f * m_delays[rvb_delay3]->getSampleDelayedBy(common::clrintf(RIGHT_TAP5_DELAY3 * M_SAMPLERATE)))
                - (0.6f * m_apfs[rvb_apf6]->getSampleDelayedBy(common::clrintf(RIGHT_TAP6_APF6 * M_SAMPLERATE)))
                - (0.6f * m_delays[rvb_delay4]->getSampleDelayedBy(common::clrintf(RIGHT_TAP7_DELAY4 * M_SAMPLERATE)));

#if defined(USE_LD)
            assert(m_mix_wet >= 0.0f && m_mix_wet <= 1.0f);
#endif
            outputs[0] = ((1.0f - m_mix_wet) * inputs[0]) + (m_mix_wet * left_out);
            outputs[1] = ((1.0f - m_mix_wet) * inputs[1]) + (m_mix_wet * right_out);
        }

#if defined(OPS_GUI)
        //----------------------------------------------------------------------------
        void setSamplerate(float samplerate)
        {
            assert(samplerate > 0.0f);
            m_samplerate = samplerate;
        }

        //----------------------------------------------------------------------------
        void clear()
        {
            for (int ii = 0; ii < NUM_RVB_DELAYS; ++ii)
            {
                m_delays[ii]->clear();
            }
        }
#endif

    private:
#if defined(OPS_GUI)
        float m_samplerate;
#endif

        // Control params
        float m_mix_wet;
        float m_predelay_len;
        float m_decay;
        float m_roomsize;
        // Gain coefficients
        float m_g1; // Decorrelates incoming signal (input diffusion 1)
        float m_g2; // (input diffusion 2)
        float m_g3; // Decorrelates tank signals; decay diffusion 2 = decay + 0.15, floor = 0.25, ceiling = 0.50 (decay diffusion 2)
        float m_g4; // Controls density of tail (decay diffusion 1)
        float m_g5; // decay (rate of decay)

        // Static delays
        vector<unique_ptr<Delay>> m_delays;

        // Allpass filters and modulated allpass filters
        vector<unique_ptr<AllPass>> m_apfs;

        // Filters for bandwidth and damping
        //OnePoleLPF *m_filters;
        vector<unique_ptr<Filter>> m_filters;

        // Modulator oscillators for modulated allpass filters
        // TODO: implement
        //        Oscillator *m_mod_osc1;
        //        Oscillator *m_mod_osc2;

        // Additional vars
        float m_feedback_left_tank;
        float m_feedback_right_tank;
};
#endif
