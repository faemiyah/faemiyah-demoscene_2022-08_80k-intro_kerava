#pragma once

#ifndef ENV_GEN_HPP
#define ENV_GEN_HPP

#include "verbatim_parameters.hpp"
#include "verbatim_common.hpp"
#include "ops_log.hpp"

enum envelope_state
{
    s_off, s_attack, s_decay, s_sustain, s_release
};

#define M_ENV_QUICK_RELEASE_COEFF 0.2f

class EnvGen
{
    public:
        //----------------------------------------------------------------------------
        EnvGen(void)
        {
#if defined(OPS_GUI)
            setSamplerate(44100.0f);
#endif
            init();
        }

        void init()
        {
            m_length = 0.1f * g_env_length_stage_max;
            m_attack = 0.0f;
            m_decay = 0.0f;
            m_sustain_level = 0.8f;
            m_release = 0.05f;
            setEnvMode(eEnvModes::linear);
            reset();
        }

        //----------------------------------------------------------------------------
        void reset(void)
        {
            m_value = 0.0f;
            m_state = s_off;
            m_quick_release = false;
            if (m_env_mode == eEnvModes::linear)
            {
                m_step = 1.0f / ((0.001f + (0.999f * m_attack)) * (M_SAMPLERATE * m_length));
            }
        }

        //----------------------------------------------------------------------------
#if defined(OPS_GUI)
        void setSamplerate(float samplerate) { m_samplerate = samplerate; };
#endif

        //----------------------------------------------------------------------------
        void setEnvMode(float value)
        {
            m_env_mode = common::clrintf(value * (eEnvModes::num_items - 1));
            // --- analog - use e^-5x
            if (m_env_mode == eEnvModes::analog)
            {
                // these are the e^-5x over/undershoot values
                // which will generate matching e^-5x curves
                // m_dAttackTCO = expf(-0.5f);    // fast attack
                m_dAttackTCO = 0.60653066f;
                //    m_dAttackTCO = exp(-5.0);
                // m_dDecayTCO = expf(-5.0f);
                m_dDecayTCO = 0.006737947f;
                m_dReleaseTCO = m_dDecayTCO;
            }
            else if (m_env_mode == eEnvModes::digital)
            {
                // digital is linear-in-dB so use
                // the -96dB noise floor for over/undershoot
                //m_dAttackTCO = powf(10.0f, -96.0f / 20.0f);
                m_dAttackTCO = 0.0000158489256f;
                m_dDecayTCO = m_dAttackTCO;
                m_dReleaseTCO = m_dDecayTCO;
                /*
                m_dAttackTCO = 0.60653066f;
                //    m_dAttackTCO = exp(-5.0);
                // m_dDecayTCO = expf(-5.0f);
                m_dDecayTCO = 0.006737947f;
                m_dReleaseTCO = m_dDecayTCO;
                */
            }
            else
            {
                // linear
                m_dAttackTCO = 1.0f;
                m_dDecayTCO = m_dAttackTCO;
                m_dReleaseTCO = m_dDecayTCO;
            }
            calculateEnv();
        }

        //----------------------------------------------------------------------------
        void setLength(float length)
        {
            m_length = length * g_env_length_stage_max;
            calculateEnv();
        }

        //----------------------------------------------------------------------------
        void setAttack(float attack)
        {
            m_attack = attack;
            calculateEnv();
        }

        //----------------------------------------------------------------------------
        void setDecay(float decay)
        {
            m_decay = decay;
            calculateEnv();
        }

        //----------------------------------------------------------------------------
        void setSustain(float sustain_level)
        {
            m_sustain_level = sustain_level;
        }

        //----------------------------------------------------------------------------
        void setRelease(float release)
        {
            m_release = release;
            calculateEnv();
        }

        //----------------------------------------------------------------------------
        void calculateEnv()
        {
            if (m_env_mode == eEnvModes::linear)
            {
                return;
            }

            // --- samples for the exponential rate
            //    float dSamples = SAMPLERATE * (m_dAttackTime_mSec / 1000.0f);
            float dSamples = M_SAMPLERATE * (m_attack * m_length);
            if (dSamples == 0.0f)
            {
                dSamples = 0.00000001f;
            }

            // --- coeff and base for iterative exponential calculation
            m_dAttackCoeff = ops_expf(-ops_logf((1.0f + m_dAttackTCO) / m_dAttackTCO) / dSamples);
            m_dAttackOffset = (1.0f + m_dAttackTCO) * (1.0f - m_dAttackCoeff);
            dSamples = M_SAMPLERATE * (m_decay * m_length);

            if (dSamples == 0.0f)
            {
                dSamples = 0.00000001f;
            }
            m_dDecayCoeff = ops_expf(-ops_logf((1.0f + m_dDecayTCO) / m_dDecayTCO) / dSamples);
            dSamples = M_SAMPLERATE * (m_release * m_length);

            if (dSamples == 0.0f)
            {
                dSamples = 0.00000001f;
            }
            m_dReleaseCoeff = ops_expf(-ops_logf((1.0f + m_dReleaseTCO) / m_dReleaseTCO) / dSamples);
        }

        //----------------------------------------------------------------------------
        void trigger(void)
        {
            m_value = 0.0f;
            m_step = 1.0f / ((0.001f + (0.999f * m_attack)) * (M_SAMPLERATE * m_length));
            m_state = s_attack;
        }

        //----------------------------------------------------------------------------
        void stop(bool quick_stop)
        {
            m_state = s_release;

            if (m_env_mode == eEnvModes::linear)
            {
                if (quick_stop)
                {
                    m_step = 1.0f / (0.002f * (M_SAMPLERATE * m_length));
                }
                else
                {
                    m_step = 1.0f / ((0.001f + (0.999f * m_release)) * (M_SAMPLERATE * m_length));
                }
            }
            else
            {
                if (quick_stop)
                {
                    m_quick_release = true;
                }
            }
        }

        //----------------------------------------------------------------------------
        bool isActive(void)
        {
            if (m_state == s_off)
            {
                return false;
            }
            return true;
        }

        //----------------------------------------------------------------------------
        float getSample(void)
        {
            switch (m_state)
            {
            case s_off:
                m_value = 0.0f;
                break;

            case s_attack:
                if (m_env_mode == eEnvModes::linear)
                {
                    m_value += m_step;
                    if (m_value > 1.0f)
                    {
                        m_value = 1.0f;
                        m_step = 1.0f / ((0.001f + (0.999f * m_decay)) * (M_SAMPLERATE * m_length));
                        m_state = s_decay;
                    }
                }
                else
                {
                    m_value = m_dAttackOffset + m_value * m_dAttackCoeff;

                    if (m_value >= 1.0f || m_attack <= 0.0f)
                    {
                        m_value = 1.0f;
                        m_state = s_decay;
                    }
                }
                break;

            case s_decay:
                if (m_env_mode == eEnvModes::linear)
                {
                    m_value -= m_step;
                    if (m_value < m_sustain_level)
                    {
                        m_value = m_sustain_level;
                        m_step = 1.0f / ((0.001f + (0.999f * m_release)) * (M_SAMPLERATE * m_length));
                        m_state = s_sustain;
                    }
                }
                else
                {
                    m_value = m_value * m_dDecayCoeff;

                    if (m_value <= m_sustain_level || m_decay <= 0.0f)
                    {
                        m_value = m_sustain_level;
                        m_state = s_sustain;
                    }
                }
                break;

            case s_sustain:
                break;

            case s_release:
                if (m_env_mode == eEnvModes::linear)
                {
                    m_value -= m_step;
                    if (m_value < 0.01f)
                    {
                        reset();
                    }
                }
                else
                {
                    if (m_quick_release)
                    {
                        m_value = m_value * M_ENV_QUICK_RELEASE_COEFF;
                    }
                    else
                    {
                        m_value = m_value * m_dReleaseCoeff;
                    }

                    if (m_value < 0.01f || m_release <= 0.0f)
                    {
                        reset();
                    }
                }
                break;

            default:
                reset();
                break;
            }

#if defined(OPS_GUI)
            assert(m_value >= 0.0f);
            assert(m_value <= 1.0f);
            if (m_value < 0.0f || m_value > 1.0f)
            {
                std::string msg = "envgen m_value out of bounds. m_value: " + std::to_string(m_value);
                throw std::runtime_error(msg);
            }
#endif
            return m_value;
        }

    private:

#if defined(OPS_GUI)
    float m_samplerate;
#endif
    // Envelope stage length in seconds (attack, decay and release parts)
    // Ranges from 0.0f to g_env_length_stage_max.
    float m_length;
    float m_step; // Steps to increment envelope value for each sample, is calculated in reset()
    // ADSR parameter values range from 0.0f to 1.0f
    float m_attack;
    float m_decay;
    float m_sustain_level;
    float m_release;
    int m_state;
    float m_value;
    int m_env_mode;
    bool m_quick_release;

    float m_dAttackOffset;

    float m_dAttackTCO;
    float m_dDecayTCO;
    float m_dReleaseTCO;

    float m_dAttackCoeff;
    float m_dDecayCoeff;
    float m_dReleaseCoeff;
};

#endif
