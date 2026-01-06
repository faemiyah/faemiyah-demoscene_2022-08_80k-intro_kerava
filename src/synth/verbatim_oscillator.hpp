#pragma once

#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"
#if defined(USE_VGL) && USE_VGL
#include "vgl/vgl_vector.hpp"
using vgl::vector;
#else
#include <vector>
using std::vector;
#endif

extern vector<float>* g_sample_buffers;

#define OSC_MAX_FREQ 20000.0f

//#define OPS_USE_MULTISAW 1

#if defined(OPS_USE_MULTISAW)
#include "verbatim_filter.hpp"
#define OPS_NUM_MULTISAW_EXTRA_OSCS 6
const float multisaw_pitch_factors[OPS_NUM_MULTISAW_EXTRA_OSCS] = { -0.11002313f, -0.06288439f, -0.01952356f, 0.01991221f, 0.06216538f, 0.10745242f };
#endif

class Oscillator
{
    public:
        //----------------------------------------------------------------------------
        Oscillator(void)
        {
#if defined(OPS_GUI)
            m_samplerate = 44100.0f;
#endif
            init();
        }

        void init()
        {
            m_samplePos = 0.0f;
            m_sampleSlot = 0;

            m_sync = true;

            m_waveform = eOscWaveforms::sine;

            m_startphase = 0.0f;
            m_phase = m_startphase;
#ifdef OPS_USE_MULTISAW
            ops_memset(m_multisaw_phases, 0, sizeof(float) * OPS_NUM_MULTISAW_EXTRA_OSCS);
            m_multisaw_detune = 1.0f;
            m_multisaw_hpf = new Filter();
            m_multisaw_hpf->setMode(eFilterModes::highpass);
            m_multisaw_mix = 0.0f;
#endif
            m_pitch = 0.0f;
            m_pitchbend_mult = 1.0f;
            m_detune = 0.0f;
            m_semi = 0.0f;
            m_pw = 0.0f;
            m_pwm = 0.0f;
            m_pw_mod = 0.0f;
            m_pitch_mod = 0.0f;
            m_phase_mod = 0.0f;

            // noise generation
            m_gaussian_noise_constant_a = 0x67452301;
            m_gaussian_noise_constant_c = static_cast<int>(0xefcdab89);
            m_sampleandhold_period = 1;
            m_sampleandhold_counter = 0;
            m_sample = 0.0f;
            m_fintmax = static_cast<float>(std::numeric_limits<int>::max());

            isActive = false;

            setOscMode(k_oscmode_oscillator);
            setDetune(0.5f);
            setSemi(0.5f);
            setVolume(1.0f);
            setPW(0.5f);
            setPWM(0.0f);
            setPitch(440.0f);
        }

        ~Oscillator(void)
        {
            // Nothing to do
        }

        //----------------------------------------------------------------------------
#if defined(OPS_GUI)
        void setSamplerate(float samplerate) { m_samplerate = samplerate; };
#endif

        //----------------------------------------------------------------------------
        void setOscMode(int oscmode)
        {
            m_oscillator_mode = oscmode;
        }

        //----------------------------------------------------------------------------
        void setWaveform(int waveform)
        {
            m_waveform = waveform;
        }

        //----------------------------------------------------------------------------
        void setVolume(float volume)
        {
            m_volume = volume;
        }

        //----------------------------------------------------------------------------
        // Used Martin Finke's modified version from http://martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
        // which is based on Tale's polyblep implementation at http://www.kvraudio.com/forum/viewtopic.php?t=375517
        float getPolyBLEP(float phase)
        {
            float phase_cycle = m_phase_increment / TAU;

            // 0 <= t < 1
            if (phase < phase_cycle)
            {
                phase /= phase_cycle;
                return (phase + phase - (phase * phase) - 1.0f);
            }
            // -1 < t < 0
            else if (phase > 1.0f - phase_cycle)
            {
                phase = (phase - 1.0f) / phase_cycle;
                return ((phase * phase) + phase + phase + 1.0f);
            }
            // 0 otherwise
            else return 0.0f;
        }

        //----------------------------------------------------------------------------
        float getSample()
        {
            if (!isActive)
            {
                return 0.0f;
            }

            float ret_val;
            float tempval;
            calculatePhaseIncrement();

            switch (m_waveform)
            {
            case eOscWaveforms::sine:
                ret_val = ops_sinf(m_phase);
                break;

            case eOscWaveforms::blepsaw:
                ret_val = (m_phase / PII) - 1.0f;
                ret_val -= getPolyBLEP(m_phase / (TAU));
                break;

            case eOscWaveforms::blepsquare:
                if (m_phase < PII)
                {
                    ret_val = 1.0f;
                }
                else
                {
                    ret_val = -1.0f;
                }
                ret_val += getPolyBLEP(m_phase / (TAU));
                ret_val -= getPolyBLEP(ops_fmodf((m_phase / (TAU)) + 0.5f, 1.0f));
                break;

            case eOscWaveforms::bleppulse:
                ret_val = (m_phase / PII) - 1.0f;
                ret_val -= getPolyBLEP(m_phase / (TAU));
                tempval = ((TAU - ops_fmodf(m_phase + m_pw + m_pw_mod, TAU)) / PII) - 1.0f;
                // TODO: figure out the other edge. probably need to modify the blep function
                ret_val += tempval;
                break;

            case eOscWaveforms::noise:
                // from http://www.musicdsp.org/showArchiveComment.php?ArchiveID=216
                m_gaussian_noise_constant_a ^= m_gaussian_noise_constant_c;
                ret_val = static_cast<float>(m_gaussian_noise_constant_c) / m_fintmax;
                m_gaussian_noise_constant_c += m_gaussian_noise_constant_a;
                break;

            case eOscWaveforms::sampleandhold:
                m_gaussian_noise_constant_a ^= m_gaussian_noise_constant_c;
                ret_val = static_cast<float>(m_gaussian_noise_constant_c) / m_fintmax;
                m_gaussian_noise_constant_c += m_gaussian_noise_constant_a;

                if (m_sampleandhold_counter < 0)
                {
                    m_sample = ret_val;
                    m_sampleandhold_counter = m_sampleandhold_period;
                }
                else
                {
                    ret_val = m_sample;
                }
                --m_sampleandhold_counter;
                break;

            case eOscWaveforms::rawsaw:
#ifdef OPS_USE_MULTISAW
                // Get the "central" osc value and adjust the amplitude using a ramp polynomial
                // NOTE: abusing the pwm value here
                ret_val = ((m_pwm * -0.55366f) + 0.99785f) * ((m_phase / PII) - 1.0f);

                for (int ii = 0; ii < OPS_NUM_MULTISAW_EXTRA_OSCS; ++ii)
                {
                    ret_val += m_multisaw_mix * ((m_multisaw_phases[ii] / PII) - 1.0f);
                }

                ret_val = m_multisaw_hpf->process(ret_val);
#else
                ret_val = (m_phase / PII) - 1.0f;
#endif
                break;

            case eOscWaveforms::rawsquare:
                if (m_phase < PII)
                {
                    ret_val = 1.0f;
                }
                else
                {
                    ret_val = -1.0f;
                }
                break;

            case eOscWaveforms::rawpulse:
                ret_val = (m_phase / PII) - 1.0f;
                ret_val += ((TAU - ops_fmodf(m_phase + m_pw + m_pw_mod, TAU)) / PII) - 1.0f;
                break;

            case eOscWaveforms::rawtriangle:
                ret_val = -1.0f + (m_phase / PII);
                ret_val = 2.0f * (fabsf(ret_val) - 0.5f);
                break;

            case eOscWaveforms::sampleplayback:
            {
                int pos = static_cast<int>(m_samplePos);
                if (pos < 0)
                {
                    pos = 0;
                }
                ret_val = g_sample_buffers[m_sampleSlot][pos];
#if defined(TEST_EXECUTION) || defined(OPS_HARMONIKKA)
                // TODO: make more efficient
#if defined(USE_VGL)
                // Compensate for the 44khz vs 48khz sample rate difference due to Opus sample decoding.
                m_samplePos += ((m_phase_increment / (TAU / M_SAMPLERATE)) / 440.0f);
#else
                m_samplePos += (m_phase_increment / (TAU / M_SAMPLERATE)) / 440.0f;
#endif
                if (m_sampleSlot > 8)
                {
                    m_sampleSlot = 8;
                }
                m_samplePos = ops_fmodf(m_samplePos, static_cast<float>(g_sample_buffers[m_sampleSlot].size()));
#else
                ret_val = 0.0f;
#endif
            }
                break;

            default:
                ret_val = 0.0f;
                break;
            }

            if (m_oscillator_mode == k_oscmode_lfo)
            {
                m_phase += m_phase_increment * (1 + m_pitch_mod);
            }
            else
            {
                m_phase += m_phase_increment + (m_phase_mod * PII);
            }

            // TODO: see if fmodf can be used
            while (m_phase > (TAU))
            {
                m_phase -= TAU;
            }

            while (m_phase < 0.0f)
            {
                m_phase += TAU;
            }

#ifdef OPS_USE_MULTISAW
            for (int ii = 0; ii < OPS_NUM_MULTISAW_EXTRA_OSCS; ++ii)
            {
                m_multisaw_phases[ii] += m_multisaw_phase_increments[ii];// +(m_phase_mod * PII);

                while (m_multisaw_phases[ii] > (TAU))
                {
                    m_multisaw_phases[ii] -= TAU;
                }

                while (m_multisaw_phases[ii] < 0.0f)
                {
                    m_multisaw_phases[ii] += TAU;
                }
            }
#endif
            return ret_val * m_volume;
        }

        //----------------------------------------------------------------------------
        void trigger()
        {
            if (m_sync == true)
            {
                m_phase = m_startphase;
            }
            isActive = true;
            m_samplePos = 0.0f;
        }

        //----------------------------------------------------------------------------
        void trigger(float pitch)
        {
            setPitch(pitch);
            trigger();
        }

        //----------------------------------------------------------------------------
        void trigger(float pitch, float startphase)
        {
            if (m_sync == true)
            {
                m_phase = startphase;
            }
            setPitch(pitch);
            isActive = true;
            m_samplePos = 0.0f;
        }

        //----------------------------------------------------------------------------
        void stop()
        {
            isActive = false;
        }

        //----------------------------------------------------------------------------
        void setDetune(float detune)
        {
            if (detune >= 0.5f)
            {
                detune *= 2.0f;
                detune -= 1.0f;
                detune *= 0.05946309436f;
                detune += 1.0f;
                m_detune = detune;
            }
            else if (detune < 0.5f)
            {
                detune *= 2.0f;
                detune *= 0.05946309436f;
                detune += 0.94387431268f;
                m_detune = detune;
            }
        }

        //----------------------------------------------------------------------------
        void setSemi(float semi)
        {
            if (semi >= 0.5f)
            {
                semi -= 0.5f;
                semi *= 2.0f;
                semi *= g_osc_semi_range;
                // tempsemi should now have the amount of semitones away from center pitch
                m_semi = ops_powf(1.05946309436f, ops_roundf(semi));
            }
            else if (semi < 0.5f)
            {
                semi = 0.5f - semi;
                semi *= 2.0f;
                semi *= g_osc_semi_range;
                // tempsemi should now have the amount of semitones away from center pitch
                m_semi = ops_powf(0.94387431268f, ops_roundf(semi));
            }
        }

        //----------------------------------------------------------------------------
        void setPW(float pw)
        {
            m_pw = TAU * pw;
#if defined(OPS_USE_MULTISAW)
            m_multisaw_detune = pw;
#endif
        }

        //----------------------------------------------------------------------------
        void setPWM(float pwm)
        {
            m_pwm = pwm;
#if defined(OPS_USE_MULTISAW)
            // Calculate the side oscillator mix coefficient here instead of calculating
            // it multiple times in getSample().
            m_multisaw_mix = (-0.73764f * pwm * pwm) + (1.2841f * pwm) + 0.044372f;
#endif
        }

        //----------------------------------------------------------------------------
        void setPWMod(float value)
        {
            m_pw_mod = TAU * m_pwm * value;
            if (m_pw_mod > TAU)
            {
                m_pw_mod -= TAU;
            }
            if (m_pw_mod < 0.0f)
            {
                m_pw_mod += TAU;
            }
        }

        //----------------------------------------------------------------------------
        void setStartphase(float startphase)
        {
            if (m_oscillator_mode == k_oscmode_lfo)
            {
                // 0-0.5 == synced to trigger, 0.5-1.0 == freerunning
                if (startphase < 0.5f)
                {
                    m_sync = true;
                    m_startphase = TAU * 2.0f * startphase;
                }
                else
                {
                    m_sync = false;
                    m_startphase = TAU * 2.0f * (startphase - 0.5f);
                }
            }
            else
            {
                m_startphase = TAU * startphase;
            }
        }

        //----------------------------------------------------------------------------
        void setPitch(float pitch)
        {
            m_pitch = pitch;
            switch (m_oscillator_mode)
            {
            case k_oscmode_oscillator:
                // limiting to arbitrary audible frequencies
                m_pitch = common::cclampf(m_pitch, -OSC_MAX_FREQ, OSC_MAX_FREQ);
                m_sampleandhold_period = static_cast<int>(OSC_MAX_FREQ / (m_pitch + 0.1f));
                break;

            case k_oscmode_lfo:
                m_pitch = m_pitch < 0.5f ? (m_pitch * 2.0f) : ((m_pitch * 2.0f) - 1.0f);
                m_pitch = g_lfo_max_frequency * common::cclampf(m_pitch, 0.0f, 1.0f);

                m_sampleandhold_period = static_cast<int>(g_lfo_max_frequency / (m_pitch + 1.0f));
                break;

            default:
                m_pitch = common::cclampf(m_pitch, -OSC_MAX_FREQ, OSC_MAX_FREQ);
                break;
            }
#ifdef OPS_USE_MULTISAW
            m_multisaw_hpf->setCutoff(m_pitch / OSC_MAX_FREQ);
#endif
        }

        //----------------------------------------------------------------------------
        void calculatePhaseIncrement()
        {
            if (m_oscillator_mode == k_oscmode_lfo)
            {
                m_phase_increment = m_pitch * (TAU / M_SAMPLERATE);
            }
            else
            {
                m_phase_increment = m_pitch * m_pitchbend_mult * (1.0f + m_pitch_mod) * m_detune * m_semi * (TAU / M_SAMPLERATE);
            }

            /*
            Oscillator nr.    Frequency(Hz) Division by center osc. Relation    1 + -offset
                1 465.7758    465.7758 / 523.3572 =    0.88997686 =    1 - 0.11002313
                2 490.4462    490.4462 / 523.3572 =    0.93711560 =    1 - 0.06288439
                3 513.1394    513.1394 / 523.3572 =    0.98047643 =    1 - 0.01952356
                4 (center osc.) 523.3572    523.3572 / 523.3572 =    1 = 1 + 0
                5 533.7784    533.7784 / 523.3572 =    1.01991221 =    1 + 0.01991221
                6 555.8919    555.8919 / 523.3572 =    1.06216538 =    1 + 0.06216538
                7 579.5932    579.5932 / 523.3572 =    1.10745242 =    1 + 0.10745242
                */

#if defined(OPS_USE_MULTISAW)
            //float pitch_mult = m_pitch * m_pitchbend_mult * m_multisaw_detune * (1.0f + m_pitch_mod) * m_detune * m_semi * (TAU / M_SAMPLERATE);
            //float pitch_mult = m_pitch * m_multisaw_detune * (TAU / M_SAMPLERATE);
            for (int ii = 0; ii < OPS_NUM_MULTISAW_EXTRA_OSCS; ++ii)
            {
                m_multisaw_phase_increments[ii] = m_phase_increment * (1.0f + (m_multisaw_detune * multisaw_pitch_factors[ii]));
            }
#endif
        }

        //----------------------------------------------------------------------------
        void setPitchBend(float value)
        {
#if defined(DNLOAD_USE_LD)
            assert(value > 0.0f);
#endif
            m_pitchbend_mult = value;
        }

        //----------------------------------------------------------------------------
        void setPitchMod(float value)
        {
            m_pitch_mod = common::cclamp1f(value);
        }

        //----------------------------------------------------------------------------
        void setPhaseMod(float value)
        {
            m_phase_mod = common::cclamp1f(value);
        }

        //----------------------------------------------------------------------------
        void setWaveform(float value)
        {
            m_waveform = common::clrintf(value * (eOscWaveforms::num_items - 1));
            m_sample = 0.0f;
        }

        //----------------------------------------------------------------------------
        void setSampleSlot(uint8_t slot)
        {
            m_sampleSlot = slot;
            if (m_sampleSlot > 8)
            {
                // The sample slots are limited to 8 at least for now.
                m_sampleSlot = 8;
            }
        }

private:
#if defined(OPS_GUI)
        float m_samplerate;
#endif
        // oscillator mode (enum oscillator_modes)
        int m_oscillator_mode;

        // oscillator waveform (enum oscillator_waveforms)
        int m_waveform;

        // oscillator phase in radians [0-2PI]
        float m_phase;

#ifdef OPS_USE_MULTISAW
        float m_multisaw_phases[OPS_NUM_MULTISAW_EXTRA_OSCS];
        float m_multisaw_phase_increments[OPS_NUM_MULTISAW_EXTRA_OSCS];
        //OnePoleLPF m_multisaw_hpf;
        Filter *m_multisaw_hpf;
#endif
        // step to increment phase in each call
        float m_phase_increment;
        // oscillator pitch in Hz
        float m_pitch;
        float m_volume;
        // detune and semi values are multipliers for the phase increment calculation
        float m_detune; // fine tune (+/- 1 semi)
        float m_semi; // tune in semitones (+/- 60)
        float m_pw; // pulsewidth
        float m_pwm; // pulsewidth modulation strength
        float m_pw_mod; // pulsewidth modulator value
        float m_startphase;
        float m_pitchbend_mult; // pitch multiplier for pitch bend
        float m_pitch_mod; // pitch modulation amount    [-1,1]
        float m_phase_mod; // phase modulation for fm    [-1,1]

#if defined(OPS_USE_MULTISAW)
        float m_multisaw_detune; // amount of detune for multisaw extra oscillators, used in place of pw
        float m_multisaw_mix; // the central/side oscillator mix coefficient, used in place of pwm. at 0 only the central osc is essentially heard
#endif

        int m_gaussian_noise_constant_a;
        int m_gaussian_noise_constant_c;
        float m_sample; // sample for sample and hold
        int m_sampleandhold_period;
        int m_sampleandhold_counter;
        float m_fintmax;
        bool isActive;
        bool m_sync;

        float m_samplePos;
        uint8_t m_sampleSlot;
};

#endif
