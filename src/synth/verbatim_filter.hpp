#pragma once

#ifndef FILTER_HPP
#define FILTER_HPP

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"
#include "verbatim_onepole_filter.hpp"
#include "ops_log.hpp"

/** \file
 *
 * Trying Robin's ZDF-SVF implementation from
 * http://www.kvraudio.com/forum/viewtopic.php?f=33&t=350246&start=105
 *
 * Also implements the one pole damping LPF as shown in
 * Jon Dattorro's Effect Design Part 1: Reverberator and Other Filters, Fig. 1
 *
 */

/* \brief Filter class.
 *
 * This is an implementation of a state variable filter using topology-preserving transform (TPT) and
 * zero-delay feedback (ZDF) technology.
 *
 * You can either use 3 outputs (lowpass, bandpass, highpass) from the SVF core by calling getOutputs() or let
 * the filter itself form a linear combination of the these 3 to obtain a desired filter mode (in addition to
 * the 3 modes above, there are also shelvers, a bell, etc.).
 */
class Filter
{
    public:
        //----------------------------------------------------------------------------
        explicit Filter(void)
        {
#if defined(OPS_GUI)
            m_samplerate = SAMPLERATE;
#endif
            setMode(eFilterModes::off);
            setCutoff(0.5f);
            setMod(0.0f, 0.0f);
            setBandwidth(2.0f);
            setDrive(0.0f);
            setOutputLevel(1.0f);
            calculateCoefficients();
            m_calc_coefficients = false;
        }

        //----------------------------------------------------------------------------
        void setMode(int mode)
        {
#if defined(OPS_GUI)
            if (mode < 0 || mode >= eFilterModes::num_items)
            {
                std::ostringstream sstr;
                sstr << "Filter::setMode: Value out of bounds: " << mode << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_mode = mode;
        }

        //----------------------------------------------------------------------------
        void setDrive(float drive)
        {
#if defined(OPS_GUI)
            if (drive < 0.0f || drive > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Filter::setDrive: Value out of bounds: " << drive << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_drive = drive;
        }

        //----------------------------------------------------------------------------
        void setOutputLevel(float output_level)
        {
#if defined(OPS_GUI)
            if (output_level < 0.0f || output_level > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Filter::setOutputLevel: Value out of bounds: " << output_level << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_output_level = output_level;
        }


        //----------------------------------------------------------------------------
#if defined(OPS_GUI)
        void setSamplerate(float samplerate)
        {
            m_samplerate = samplerate;
            calculateCoefficients();
        }
#endif

        //----------------------------------------------------------------------------
        void setMod(float amount, float modifier)
        {
            // amount (envmod) range is from -100% to 100% [-1.0,1.0]
            // modifier (envelope) range is [0.0,1.0] where 0.0 should be base freq

            // clamp
            amount = common::cclamp1f(amount);

            m_mod = amount * modifier;
            m_calc_coefficients = true;
        }

        //----------------------------------------------------------------------------
        void setStaticMod(float amount)
        {
            // clamp
            amount = common::cclamp1f(amount);

            m_staticmod = amount;
            m_calc_coefficients = true;
        }

        //----------------------------------------------------------------------------
        // Sets the characteristic frequency (which is the cutoff-frequency for lowpass and highpass,
        // the center-frequency for bandpass, bandstop and bell, and the halfgain-frequency for
        // shelvers).
        void setCutoff(float cutoff)
        {
#if defined(OPS_GUI)
            if (cutoff < 0.0f || cutoff > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Filter::setCutoff: Value out of bounds: " << cutoff << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_base_freq = cutoff * cutoff;
            m_calc_coefficients = true;
        }

        //----------------------------------------------------------------------------
        // Sets the resonance gain (as linear gain factor) for low-, high- and (constant skirt gain)
        // bandpass filters or the boost/cut gain for bell- and shelving filters.
        void setResonance(float resonance)
        {
#if defined(OPS_GUI)
            if (resonance < 0.0f || resonance > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Filter::setResonance: Value out of bounds: " << resonance << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_resonance = ONE_OVER_SQRT2 + 99.0f * (resonance * resonance * resonance);
            setBandwidth(40.0f * (resonance * resonance) + 0.001f);
            m_calc_coefficients = true;
        }

        //----------------------------------------------------------------------------
        // When you use getOutputs() directly to obtain the lowpass, bandpass and highpass signals
        // of the core SVF, this function returns the value by which the bandpass signal has to be scaled
        // in order to achieve lowpass + scaler*bandpass + highpass == original input.
        inline float getBandpassScaler() const { return m_r2; }

        //----------------------------------------------------------------------------
        // Sets the bandwidth (in octaves) for (constant peak gain) bandpass filters and bell filters.
        // In the case of shelving filters, this also determines the slope at the halfgain point.
        // At B = (2*asinh(1/sqrt(2)))/log(2) = 1.899968626952992, the slope is as steep as it can be
        // without overshooting.
        void setBandwidth(float bandwidth)
        {
#if defined(OPS_GUI)
            if (bandwidth < 0.0f || bandwidth > 40.1f)
            {
                std::ostringstream sstr;
                sstr << "Filter::setBandwidth: Value out of bounds: " << bandwidth << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_bandwidth = bandwidth;
        }

        //----------------------------------------------------------------------------
        float process(float in)
        {
            common::add_dc(in);
            if (m_mode == eFilterModes::off)
            {
                return in;
            }

            float out;
            if (m_mode != eFilterModes::only_drive)
            {
                // SVF process implementation
                if (m_calc_coefficients)
                {
                    calculateCoefficients();
                    m_calc_coefficients = false;
                }
                float yL, yB, yH;
                getOutputs(in, yL, yB, yH);
                out = (m_c_low * yL + m_c_band * yB + m_c_high * yH);
            }
            else
            {
                out = in;
            }

            if (m_drive > 0.001f)
            {
                out = ops_tanhf(out * (1.0f + (3.0f * m_drive)));
            }
            return m_output_level * out;
        }

        //----------------------------------------------------------------------------
        // svf implementation
        void calculateCoefficients()
        {
            m_cutoff = 0.5f * M_SAMPLERATE * (m_base_freq + m_mod + m_staticmod);

            // avoiding going near nyquist as the filter will blow
            m_cutoff = common::cclampf(m_cutoff, 20.0f, M_SAMPLERATE * 0.495f);
            m_g = ops_tanf(PII / M_SAMPLERATE * m_cutoff); // embedded integrator gain (Fig 3.11)

            switch (m_mode)
            {
            default:
            case eFilterModes::off:
                m_r2 = 1.0f / m_resonance; // can we use an arbitrary value here, for example R2 = 1?
                m_c_low = 1.0f;
                m_c_band = getBandpassScaler();
                m_c_high = 1.0f;
                break;

            case eFilterModes::lowpass:
                m_r2 = 1.0f / m_resonance;
                m_c_low = 1.0f; m_c_band = 0.0f; m_c_high = 0.0f;
                break;

            case eFilterModes::highpass:
                m_r2 = 1.0f / m_resonance;
                m_c_low = 0.0f; m_c_band = 0.0f; m_c_high = 1.0f;
                break;

                //        case BANDPASS_SKIRT:
            case eFilterModes::bandpass_skirt:
                m_r2 = 1.0f / (1.00001f + m_bandwidth);
                m_c_low = 0.0f;
                m_c_band = 1.0f;
                m_c_high = 0.0f;
                break;

                //        case BANDPASS_PEAK:
            case eFilterModes::bandpass_peak:
                // tweaked by ear, hence magic numbers
                m_r2 = 2.0f * bandwidthToR(40.001f - 0.9654f * m_bandwidth);
                m_c_low = 0.0f;
                m_c_band = m_r2;
                m_c_high = 0.0f;
                break;

            case eFilterModes::bandstop:
                m_r2 = 2.0f * bandwidthToR(m_bandwidth);
                m_c_low = 1.0f;
                m_c_band = 0.0f;
                m_c_high = 1.0f;
                break;

                /*
                case BELL:
                {
                double fl = fc*pow(2, -B/2); // lower bandedge frequency (in Hz)
                double wl = tan(PII*fl/fs); // warped radian lower bandedge frequency /(2*fs)
                double r= g/wl; r *= r;// warped frequency ratio wu/wl == (wc/wl)^2 where wu is the
                // warped upper bandedge, wc the center
                m_r2 = 2*sqrt(((r*r+1)/r-2)/(4*m_resonance));
                cL = 1; cB = m_r2*m_resonance; cH = 1;
                }
                break;
                case LOWSHELF:
                {
                double A = sqrt(m_resonance);
                g /= sqrt(A); // scale SVF-cutoff frequency for shelvers
                m_r2 = 2*sinh(B*log(2.0)/2);
                cL = m_resonance; cB = m_r2*A; cH = 1;
                }
                break;
                case HIGHSHELF:
                {
                double A = sqrt(m_resonance);
                g *= sqrt(A); // scale SVF-cutoff frequency for shelvers
                m_r2 = 2*sinh(B*log(2.0)/2);
                cL = 1; cB = m_r2*A; cH = m_resonance;
                }
                break;
                case ALLPASS:
                {
                m_r2 = 2*bandwidthToR(B);
                cL = 1; cB = -m_r2; cH = 1;
                }
                break;
                */
            }
            m_h = 1.0f / (1.0f + m_r2 * m_g + m_g * m_g); // factor for feedback precomputation
        }

        //----------------------------------------------------------------------------
        void setMode(float value)
        {
#if defined(OPS_GUI)
            if (value < 0.0f || value > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Filter::setMode: Value out of bounds: " << value<< std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif

            setMode(common::clrintf(value * static_cast<float>(eFilterModes::num_items - 1)));
        }

        //----------------------------------------------------------------------------
        // Returns the 3 outputs (lowpass, bandpass, highpass) of the core SVF.
        void getOutputs(float in, float &yL, float &yB, float &yH)
        {
            common::add_dc(m_state1);
            common::add_dc(m_state2);
            // compute highpass output via Eq. 5.1:
            yH = (in - m_r2 * m_state1 - m_g * m_state1 - m_state2) * m_h;

            // compute bandpass output by applying 1st integrator to highpass output:
            yB = m_g * yH + m_state1;
            m_state1 = m_g * yH + yB; // state update in 1st integrator

            // compute lowpass output by applying 2nd integrator to bandpass output:
            yL = m_g * yB + m_state2;
            m_state2 = m_g * yB + yL; // state update in 2nd integrator

            // Remark: we have used two TDF2 integrators (Fig. 3.11) where one of them would be in code:
            // y = g*x + s; // output computation
            // s = g*x + y; // state update

            // as a cheap trick to introduce nonlinear behavior, we apply a nonlinearity to the states of
            // the integrators (uncomment, if you want that):
            //s1 = tanh(s1);
            //s2 = tanh(s2);
        }

        //----------------------------------------------------------------------------
        // Computes damping coefficient R from desired bandwidth and the prewarped radian center
        // frequency (for bandpass with constant peak gain, bandstop and allpass).
        float bandwidthToR(float bandwidth)
        {
            float fl = m_cutoff * ops_exp2f(bandwidth * -0.5f);    // lower bandedge frequency (in Hz)

            float gl = ops_tanf(PII * fl / M_SAMPLERATE); // warped radian lower bandedge frequency /(2*fs)
            float r = gl / m_g;            // ratio between warped lower bandedge- and center-frequencies
            // unwarped: r = pow(2, -bandwidth/2) -> approximation for low
            // center-frequencies
            return ops_sqrtf((1.0f - r * r) * (1.0f - r * r) / (4.0f * r * r));
        }

        //----------------------------------------------------------------------------
        // Resets the internal state buffers to zero.
        void reset()
        {
            m_state1 = m_state2 = 0.0f;
        }

private:
#if defined(OPS_GUI)
        float m_samplerate = 44100.0f;
#endif

        int m_mode = 0;
        /// the real internal cutoff freq, composed from base freq and mod
        float m_cutoff = 0.0f;
        /// gain
        float m_resonance = ONE_OVER_SQRT2;
        /// for env mod
        float m_mod = 0.0f;
        /// for vel mod & keytrack
        float m_staticmod = 0.0f;
        float m_base_freq = 0.0f;
        float m_drive = 0.0f;
        float m_output_level = 0.0f;

        /// in octaves
        float m_bandwidth = 2.0f;

        // states
        float m_state1 = 0.0f;
        float m_state2 = 0.0f;

        // coefficients (robin)
        /// embedded integrator gain/
        float m_g = 0.0f;
        /// twice the damping coefficient (R2 == 2*R == 1/Q)
        float m_r2 = 1 / ONE_OVER_SQRT2;
        /// factor for feedback (== 1/(1+2*R*g+g*g))
        float m_h = 0.5f;
        /// coefficient for lowpass signals
        float m_c_low = 0.0f;
        /// coefficient for bandpass signals
        float m_c_band = 0.0f;
        /// coefficient for highpass signals
        float m_c_high = 0.0f;

        /// flag to trigger coefficient calculation during process
        bool m_calc_coefficients = false;
};

#endif
