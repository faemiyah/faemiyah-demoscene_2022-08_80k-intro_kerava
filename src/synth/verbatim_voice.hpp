#pragma once

#ifndef VOICE_HPP
#define VOICE_HPP

#include "verbatim_common.hpp"
#include "verbatim_filter.hpp"
#include "verbatim_env_gen.hpp"
#include "verbatim_oscillator.hpp"
#include "verbatim_parameters.hpp"
#include "ops_log.hpp"

#define NUM_OSCS 3
#define NUM_LFOS 3
#define NUM_FILTERS 2
#define NUM_ENVS 3

#if defined(USE_VGL) && USE_VGL
#include "vgl/vgl_unique_ptr.hpp"
#include "vgl/vgl_vector.hpp"
using vgl::unique_ptr;
using vgl::vector;
#else
#include <memory>
using std::shared_ptr;
using std::unique_ptr;
#include <vector>
using std::vector;
#endif

static float g_note2freq_table[128];
static bool g_is_note2freq_table_generated = false;

static void generateNote2FreqTable()
{
    for (int ii = 0; ii < 128; ++ii)
    {
        // f = 2 ^ ((d - 69) / 12) * 440.0f;
        g_note2freq_table[ii] = ops_exp2f((static_cast<float>(ii) - 69.0f) / 12.0f) * 440.0f;
    }
    g_is_note2freq_table_generated = true;
}

static float note2Freq(int note)
{
    if (note < 0)
    {
        note = 0;
    }
    else if (note > 127)
    {
        note = 127;
    }

    return g_note2freq_table[note];
}

//----------------------------------------------------------------------------
// Returns the note number closest to the frequency parameter.
// The cutoff point is halfway between notes in Hz.
static int getNoteClosestToFreq(float freq)
{
    for (int ii = 0; ii < 127; ++ii)
    {
        if (freq >= g_note2freq_table[ii])
        {
            if (freq < g_note2freq_table[ii + 1])
            {
                if (freq < (0.5f * (g_note2freq_table[ii] + g_note2freq_table[ii + 1])))
                {
                    return ii;
                }
                else
                {
                    return ii + 1;
                }
            }
        }
    }
    return 127;
}


/** \brief Voice class, handles generation for a single voice.
 *
 * Idea and parts of code used from Niall Moody's Polyphonic Synth Tutorial at
 * http://www.niallmoody.com/ndcplugs/programming.htm
 *
 * Glide / portamento logic adapted from Will Pirkle's Designing Software Synthesizer Plug-Ins in
 * C++ (Focal Press 2015) example projects
 *
 */

class Voice
{
    struct FilterMods
    {
        float env_mod;
        float vel_mod;
        float keytrack;
    };

    struct LfoMods
    {
        int mod_dest;
        float mod_amount;
        float keytrack;
    };

    public:
        //----------------------------------------------------------------------------
        Voice()
        {
#if defined(OPS_GUI)
            m_samplerate = SAMPLERATE;
            m_soft_pedal_state = false;
            m_trigger_lfo3_on_pedal = false;
#endif
            init();
        }

        void init()
        {
            if (g_is_note2freq_table_generated == false)
            {
                generateNote2FreqTable();
            }
            int ii = 0;

            for (ii = 0; ii < NUM_OSCS; ++ii)
            {
                m_oscs.emplace_back(new Oscillator());
                m_osc_ratios.emplace_back(0.0f);
            }
            for (ii = 0; ii < NUM_LFOS; ++ii)
            {
                m_lfos.emplace_back(new Oscillator());
                m_lfo_mods.emplace_back(new LfoMods());
            }
            for (ii = 0; ii < NUM_FILTERS; ++ii)
            {
                m_filters.emplace_back(new Filter());
                m_filter_mods.emplace_back(new FilterMods());
            }
            for (ii = 0; ii < NUM_ENVS; ++ii)
            {
                m_envs.emplace_back(new EnvGen());
            }

            m_synth_type = eSynthTypes::substractive;
            m_amp_env_vel_mod = 0.0f;
            m_frequency = 440.0f;
            m_glissando_frequency = m_frequency;
            m_note = -1;
            m_velocity = 0.0f;
            m_pitch_bend = 0.5f;
            m_is_active = false;
            m_note_on = false;
            m_filter_keytrack_offset_freq = 0.0f;
            m_pitch_env_mod = 0.0f;
            m_glide_type = eGlideTypes::off;
            m_glide_time = 0.0f;
            m_glide_counter = 0.0f;
            m_glide_increment = 0.0f;
            m_glide_start_frequency = 0.0f;
            m_glide_target_frequency = 0.0f;
            m_lfo_keytrack_offset = 0.0f;
            m_filter_routing = eFilterRoutings::serial;
            m_osc3_feedback = 0.0f;

            for (ii = 0; ii < NUM_FILTERS; ++ii)
            {
                m_filter_mods[ii]->env_mod = 0.75f;
                m_filter_mods[ii]->vel_mod = 0.75f;
                m_filter_mods[ii]->keytrack = 0.75f;
            }

            for (ii = 0; ii < NUM_LFOS; ++ii)
            {
                m_lfos[ii]->setOscMode(k_oscmode_lfo);
                m_lfos[ii]->setWaveform(eOscWaveforms::sine);
                m_lfos[ii]->trigger(0.1f);

                m_lfo_mods[ii]->keytrack = 0.0f;
                m_lfo_mods[ii]->mod_dest = 0;
                m_lfo_mods[ii]->mod_amount = 0.0f;
            }
        }

        //----------------------------------------------------------------------------
        ~Voice()
        {
            noteOff(true);
        }

        //----------------------------------------------------------------------------
        // Called when the PolyHandler assigns a new note to this voice.
        // We set the voice to be active in this method, and set up various
        // variables we need to start outputting sound.
        // A negative prev_note will indicate that the note is to have no
        // portamento or glissando.
        // A negative velocity will indicate that we should use the velocity
        // already set in a previous note and skip triggering the envelopes.
        void noteOn(int note, int prev_note, float velocity)
        {
            m_glide_target_frequency = note2Freq(note);
            if (!m_note_on)
            {
                // new note
                // calculate multiplier etc. for portamento/glissando if enabled, otherwise just set the new note
                if ((m_glide_type != eGlideTypes::off)
                    && m_glide_increment > 0.0f && prev_note >= 0)
                {
                    // --- save start frequency
                    m_glide_start_frequency = note2Freq(prev_note < 0 ? note : prev_note);

                    // --- set osc start pitch
                    m_frequency = m_glide_start_frequency;
                }
                else
                {
                    // --- no portamento; set final pitch
                    m_frequency = m_glide_target_frequency;
                }

                int ii;
                for (ii = 0; ii < NUM_OSCS; ++ii)
                {
                    m_oscs[ii]->trigger();
                }

                for (ii = 0; ii < NUM_LFOS; ++ii)
                {
                    m_lfos[ii]->trigger();
                }
#if defined(OPS_GUI)
                if (m_trigger_lfo3_on_pedal && !m_soft_pedal_state)
                {
                    m_lfos[2]->stop();
                }
#endif
            }
            else // retriggering or stealing a note
            {
                if ((m_glide_type != eGlideTypes::off)
                    && m_glide_increment > 0.0f && prev_note >= 0)
                {
                    // note stealing / retrigger case with portamento or legato
                    if (m_glide_counter > 0.0f)
                    {
                        // already gliding, adapt
                        m_glide_start_frequency = m_frequency * getGlideMultiplier(m_glide_counter * getNotesBetweenFrequencies(m_frequency, m_glide_target_frequency));
                    }
                    else
                    {
                        m_glide_start_frequency = note2Freq(prev_note);
                    }

                    m_frequency = m_glide_start_frequency;
                }
                else if (m_pitch_env_mod > 0.0f || m_pitch_env_mod < 0.0f)
                {
                    // If we have pitch envelopes, we are probably doing
                    // percussive noises and retriggering is in order
                    // without changing the frequency
                    for (int ii = 0; ii < NUM_OSCS; ++ii)
                    {
                        m_oscs[ii]->trigger();
                    }
                }
                else
                {
                    m_frequency = m_glide_target_frequency;
                    for (int ii = 0; ii < NUM_OSCS; ++ii)
                    {
                        m_oscs[ii]->setPitch(m_frequency);
                    }
                }
            }

            // reset counter
            m_glide_counter = 0.0f;

            m_filter_keytrack_offset_freq = m_frequency - note2Freq(64);
            m_note = note;
            for (int ii = 0; ii < NUM_LFOS; ++ii)
            {
                m_lfos[ii]->setPitchMod(m_lfo_mods[ii]->keytrack * (static_cast<float>(note - 64) / 64.0f));
            }
            m_is_active = true;
            m_note_on = true;
            if (velocity > 0.0f)
            {
                m_velocity = velocity;
                for (int ii = 0; ii < NUM_ENVS; ++ii)
                {
                    m_envs[ii]->trigger();
                }
            }
        }

        //----------------------------------------------------------------------------
        // Pushes the voice's envelope into it's release section here, so the voice can
        // come to a halt accordingly.
        // The quick_stop parameter will trigger an accelerated release envelope in
        // order to quickly stop the voice but attempting to minimise clicks from abrupt
        // stops.
        void noteOff(bool quick_stop)
        {
            for (int ii = 0; ii < NUM_ENVS; ++ii)
            {
                m_envs[ii]->stop(quick_stop);
            }
            m_note_on = false;
        }

        //----------------------------------------------------------------------------
        // Used by PolyHandler to determine whether to call getSample() for
        // this voice.
        bool getIsActive()
        {
            return m_is_active;
        }

#if defined(OPS_GUI)
        //----------------------------------------------------------------------------
        // Used by PolyHandler to determine whether the voice has been sent the noteoff
        // message already. Only used for GUI updates.
        bool getIsReleased()
        {
            return !m_note_on;
        }
#endif

        //----------------------------------------------------------------------------
        int getCurrentNote()
        {
            return m_note;
        }

#if defined(OPS_GUI)
        //----------------------------------------------------------------------------
        void setSamplerate(float samplerate)
        {
            m_samplerate = samplerate;
        }

        void setSoftPedalState(bool soft_pedal_state)
        {
            if (m_is_active && m_trigger_lfo3_on_pedal)
            {
                if (soft_pedal_state)
                {
                    m_lfos[2]->trigger();
                }
                else
                {
                    m_lfos[2]->stop();
                }
            }
            m_soft_pedal_state = soft_pedal_state;
        }
#endif

        //----------------------------------------------------------------------------
        void setPitchBend(float val)
        {
            val = common::cclampf(val, 0.0f, 1.0f);

            // Scale pitch bend to +/- g_pitch_bend_range semitones.
            val -= 0.5f;
            val *= 2.0f;
            m_pitch_bend = ops_exp2f((val * g_pitch_bend_range) / 12.0f);
            for (int ii = 0; ii < NUM_OSCS; ++ii)
            {
                m_oscs[ii]->setPitchBend(m_pitch_bend);
            }
        }

        //----------------------------------------------------------------------------
        void setPitchEnvMod(float val)
        {
            // Scale pitch mod to +/- the amount of semitones set in parameters.h -> g_pitch_env_range.
            if (val >= 0.5f)
            {
                val -= 0.5f;
                val *= 2.0f;
            }
            else if (val < 0.5f)
            {
                val = 0.5f - val;
                val *= -2.0f;
            }
            m_pitch_env_mod = val;
        }

        float getScaledPitchEnvModMultiplier()
        {
            if (m_pitch_env_mod > 0.0f)
            {
                return m_pitch_env_mod * (note2Freq(m_note) * ops_powf(1.05946309436f, static_cast<float>(g_pitch_env_range)));
            }
            else if (m_pitch_env_mod < 0.0f)
            {
                return m_pitch_env_mod * (note2Freq(m_note) * ops_powf(0.94387431268f, static_cast<float>(g_pitch_env_range)) + note2Freq(m_note));
            }
            return 0.0f;
        }

        //----------------------------------------------------------------------------
        void setFilterRouting(float val)
        {
            m_filter_routing = common::clrintf(val * (eFilterRoutings::num_items - 1));
        }

        //----------------------------------------------------------------------------
        void setGlideTime(float val)
        {
            m_glide_time = val;
            if (val == 0.0f)
            {
                m_glide_increment = 1.0f;
            }
            else
            {
                m_glide_increment = 1000.0f / val / M_SAMPLERATE;
            }
        }

        //----------------------------------------------------------------------------
        float getGlideMultiplier(float note_difference)
        {
            if (note_difference == 0.0f)
            {
                return 1.0f;
            }

            // 2^(N/12)
            return ops_exp2f(note_difference / 12.0f);
        }

        //----------------------------------------------------------------------------
        float getNotesBetweenFrequencies(float start_freq, float end_freq)
        {
            // n    =    12*log2(f2/f1).
            return 12.0f * ops_log2f(end_freq / start_freq);
        }

        //----------------------------------------------------------------------------
        void setParameter(int parameter, float value)
        {
#if defined(OPS_GUI)
            if (value < 0.0f || value > 1.0f)
            {
                if (parameter != eSSynth::k_tempo && parameter != eSSynth::k_samplerate)
                {
                    std::ostringstream sstr;
                    sstr << "Error: Voice::setParameter Param: " << parameter << " Value out of bounds: " << value << std::endl;
                    throw std::runtime_error(sstr.str());
                }
            }
#endif
            // Pass parameters to components affected
            switch (parameter)
            {
            case eSSynth::k_pitchbend:
                setPitchBend(value);
                break;

            case eSSynth::k_filter_routing:
                setFilterRouting(value);
                break;

                // osc1
            case eSSynth::k_osc1_waveform:
                m_oscs[0]->setWaveform(value);
                break;

            case eSSynth::k_osc1_detune:
                m_oscs[0]->setDetune(value);
                break;

            case eSSynth::k_osc1_semi:
                m_oscs[0]->setSemi(value);
                break;

            case eSSynth::k_osc1_volume:
                m_oscs[0]->setVolume(value);
                break;

            case eSSynth::k_osc1_pw:
                m_oscs[0]->setPW(value);
                break;

            case eSSynth::k_osc1_pwm:
                m_oscs[0]->setPWM(value);
                break;

            case eSSynth::k_osc1_ratio:
                m_osc_ratios[0] = value * g_max_fm_ratio;
                m_oscs[0]->setSampleSlot(static_cast<uint8_t>(m_osc_ratios[0]));
                break;

                // osc2
            case eSSynth::k_osc2_waveform:
                m_oscs[1]->setWaveform(value);
                break;

            case eSSynth::k_osc2_detune:
                m_oscs[1]->setDetune(value);
                break;

            case eSSynth::k_osc2_semi:
                m_oscs[1]->setSemi(value);
                break;

            case eSSynth::k_osc2_volume:
                m_oscs[1]->setVolume(value);
                break;

            case eSSynth::k_osc2_pw:
                m_oscs[1]->setPW(value);
                break;

            case eSSynth::k_osc2_pwm:
                m_oscs[1]->setPWM(value);
                break;

            case eSSynth::k_osc2_ratio:
                m_osc_ratios[1] = value * g_max_fm_ratio;
                m_oscs[1]->setSampleSlot(static_cast<uint8_t>(m_osc_ratios[1]));
                break;

                // osc3
            case eSSynth::k_osc3_waveform:
                m_oscs[2]->setWaveform(value);
                break;

            case eSSynth::k_osc3_detune:
                m_oscs[2]->setDetune(value);
                break;

            case eSSynth::k_osc3_semi:
                m_oscs[2]->setSemi(value);
                break;

            case eSSynth::k_osc3_volume:
                m_oscs[2]->setVolume(value);
                break;

            case eSSynth::k_osc3_pw:
                m_oscs[2]->setPW(value);
                break;

            case eSSynth::k_osc3_pwm:
                m_oscs[2]->setPWM(value);
                break;

            case eSSynth::k_osc3_ratio:
                m_osc_ratios[2] = value * g_max_fm_ratio;
                m_oscs[2]->setSampleSlot(static_cast<uint8_t>(m_osc_ratios[2]));
                break;

            case eSSynth::k_osc3_feedback:
                m_osc3_feedback = value;
                break;

                // lfo1
            case eSSynth::k_lfo1_waveform:
                m_lfos[0]->setWaveform(value);
                break;

            case eSSynth::k_lfo1_speed:
                m_lfos[0]->setPitch(value);
                break;

            case eSSynth::k_lfo1_startphase:
                m_lfos[0]->setStartphase(value);
                break;

            case eSSynth::k_lfo1_keytrack:
                m_lfo_mods[0]->keytrack = (value - 0.5f) * 2.0f;
                break;

            case eSSynth::k_lfo1_mod_dest:
                m_lfo_mods[0]->mod_dest = common::clrintf(value * static_cast<float>(eModDests::num_items - 1));
                break;

            case eSSynth::k_lfo1_mod_amount:
                m_lfo_mods[0]->mod_amount = value;
                break;

                // lfo2
            case eSSynth::k_lfo2_waveform:
                m_lfos[1]->setWaveform(value);
                break;

            case eSSynth::k_lfo2_speed:
                m_lfos[1]->setPitch(value);
                break;

            case eSSynth::k_lfo2_startphase:
                m_lfos[1]->setStartphase(value);
                break;

            case eSSynth::k_lfo2_keytrack:
                m_lfo_mods[1]->keytrack = (value - 0.5f) * 2.0f;
                break;

            case eSSynth::k_lfo2_mod_dest:
                m_lfo_mods[1]->mod_dest = common::clrintf(value * static_cast<float>(eModDests::num_items - 1));
                break;

            case eSSynth::k_lfo2_mod_amount:
                m_lfo_mods[1]->mod_amount = value;
                break;

                // lfo3
            case eSSynth::k_lfo3_waveform:
                m_lfos[2]->setWaveform(value);
                break;

            case eSSynth::k_lfo3_speed:
                m_lfos[2]->setPitch(value);
                break;

            case eSSynth::k_lfo3_startphase:
                m_lfos[2]->setStartphase(value);
#if defined (OPS_GUI)
                // Ugly, remove after implementing a separate control for lfo start delays / pedal triggers.
                // If the start phase for lfo3 is low enough, only run lfo3 when the soft pedal is held down. Very gimmicky.
                if (value < 0.01)
                {
                    m_trigger_lfo3_on_pedal = true;
                }
#endif
                break;

            case eSSynth::k_lfo3_keytrack:
                m_lfo_mods[2]->keytrack = (value - 0.5f) * 2.0f;
                break;

            case eSSynth::k_lfo3_mod_dest:
                m_lfo_mods[2]->mod_dest = common::clrintf(value * static_cast<float>(eModDests::num_items - 1));
                break;

            case eSSynth::k_lfo3_mod_amount:
                m_lfo_mods[2]->mod_amount = value;
                break;

                // env mode / shape
            case eSSynth::k_env_mode:
                for (int ii = 0; ii < NUM_ENVS; ++ii)
                {
                    m_envs[ii]->setEnvMode(value);
                }
                break;

                // pitch env
            case eSSynth::k_pitch_env_mod:
                setPitchEnvMod(value);
                break;

            case eSSynth::k_pitch_attack:
                m_envs[k_pitch_env]->setAttack(value);
                break;

            case eSSynth::k_pitch_decay:
                m_envs[k_pitch_env]->setDecay(value);
                break;

            case eSSynth::k_pitch_sustain:
                m_envs[k_pitch_env]->setSustain(value);
                break;

            case eSSynth::k_pitch_release:
                m_envs[k_pitch_env]->setRelease(value);
                break;

                // amp env
            case eSSynth::k_amp_attack:
                m_envs[k_amp_env]->setAttack(value);
                break;

            case eSSynth::k_amp_decay:
                m_envs[k_amp_env]->setDecay(value);
                break;

            case eSSynth::k_amp_sustain:
                m_envs[k_amp_env]->setSustain(value);
                break;

            case eSSynth::k_amp_release:
                m_envs[k_amp_env]->setRelease(value);
                break;

            case eSSynth::k_amp_env_vel_mod:
                m_amp_env_vel_mod = value;
                break;

            case eSSynth::k_env_length:
                for (int ii = 0; ii < NUM_ENVS; ++ii)
                {
                    m_envs[ii]->setLength(value);
                }
                break;

                // filter 1
            case eSSynth::k_filter1_mode:
                m_filters[0]->setMode(value);
                break;

            case eSSynth::k_filter1_cutoff:
                m_filters[0]->setCutoff(value);
                break;

            case eSSynth::k_filter1_resonance:
                m_filters[0]->setResonance(value);
                break;

            case eSSynth::k_filter1_keytrack:
                m_filter_mods[0]->keytrack = (value - 0.5f) * 2.0f;
                break;

            case eSSynth::k_filter1_env_mod:
                m_filter_mods[0]->env_mod = (value - 0.5f) * 2.0f;
                m_filter_mods[0]->env_mod *= m_filter_mods[0]->env_mod;
                break;

            case eSSynth::k_filter1_vel_mod:
                m_filter_mods[0]->vel_mod = (value - 0.5f) * 2.0f;
                m_filter_mods[0]->vel_mod *= m_filter_mods[0]->vel_mod;
                break;

            case eSSynth::k_filter1_drive:
                m_filters[0]->setDrive(value);
                break;

            case eSSynth::k_filter1_output_level:
                m_filters[0]->setOutputLevel(value);
                break;

                // filter 2
            case eSSynth::k_filter2_mode:
                m_filters[1]->setMode(value);
                break;

            case eSSynth::k_filter2_cutoff:
                m_filters[1]->setCutoff(value);
                break;

            case eSSynth::k_filter2_resonance:
                m_filters[1]->setResonance(value);
                break;

            case eSSynth::k_filter2_keytrack:
                m_filter_mods[1]->keytrack = (value - 0.5f) * 2.0f;
                break;

            case eSSynth::k_filter2_env_mod:
                m_filter_mods[1]->env_mod = (value - 0.5f) * 2.0f;
                m_filter_mods[1]->env_mod *= m_filter_mods[1]->env_mod;
                break;

            case eSSynth::k_filter2_vel_mod:
                m_filter_mods[1]->vel_mod = (value - 0.5f) * 2.0f;
                m_filter_mods[1]->vel_mod *= m_filter_mods[1]->vel_mod;
                break;

            case eSSynth::k_filter2_drive:
                m_filters[1]->setDrive(value);
                break;

            case eSSynth::k_filter2_output_level:
                m_filters[1]->setOutputLevel(value);
                break;

                // filter envelope
            case eSSynth::k_filter_attack:
                m_envs[k_filter_env]->setAttack(value);
                break;

            case eSSynth::k_filter_decay:
                m_envs[k_filter_env]->setDecay(value);
                break;

            case eSSynth::k_filter_sustain:
                m_envs[k_filter_env]->setSustain(value);
                break;

            case eSSynth::k_filter_release:
                m_envs[k_filter_env]->setRelease(value);
                break;

            case eSSynth::k_glide_type:
                m_glide_type = common::clrintf(value * static_cast<float>(eGlideTypes::num_items - 1));
                break;

            case eSSynth::k_glide_time:
                setGlideTime(value * static_cast<float>(g_max_glide_time));
                break;

            case eSSynth::k_synth_type:
                m_synth_type = common::clrintf(value * static_cast<float>(eSynthTypes::num_items - 1));
                break;

            default:
                break;
            }
        }

        //----------------------------------------------------------------------------
        // Called for every sample in the plugin's processing loop (when the
        // voice is active).
        // In this method we generate the next sample to output.
        // The envelope should take care of itself, moving onto the next section
        // when necessary, and telling the voice when it's reached the end of
        // it's release section.    When this happens, the voice sets itself to be
        // inactive, and the PolyHandler will not call getSample() again until a
        // new note has been assigned to the voice with noteOn().
        float getSample()
        {
            float retVal = 0.0f;
            float temp = 0.0f;

            float amp_mod = 0.0f;
            float amp_mods[NUM_OSCS];

            float lfopitch_mod = 0.0f;
            float lfopitch_mods[NUM_OSCS];

            float filter_mods[NUM_FILTERS];

            for (int ii = 0; ii < NUM_OSCS; ++ii)
            {
                amp_mods[ii] = 0.0f;
            }
            for (int ii = 0; ii < NUM_OSCS; ++ii)
            {
                lfopitch_mods[ii] = 0.0f;
            }
            for (int ii = 0; ii < NUM_FILTERS; ++ii)
            {
                filter_mods[ii] = 0.0f;
            }

            // This is pretty terrible, probably should find a better way

            float lfo_vals[NUM_LFOS];
            int ii;
            for (ii = 0; ii < NUM_LFOS; ++ii)
            {
                lfo_vals[ii] = m_lfos[ii]->getSample();

                if (m_lfo_mods[ii]->mod_dest != eModDests::none)
                {
                    switch (m_lfo_mods[ii]->mod_dest)
                    {
                    case eModDests::pitch:
                        lfopitch_mod += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::pitch_osc1:
                        lfopitch_mods[0] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::pitch_osc2:
                        lfopitch_mods[1] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::pitch_osc3:
                        lfopitch_mods[2] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::amp:
                        amp_mod += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::amp_osc1:
                        amp_mods[0] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::amp_osc2:
                        amp_mods[1] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::amp_osc3:
                        amp_mods[2] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::filter1_cutoff:
                        filter_mods[0] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::filter2_cutoff:
                        filter_mods[1] += m_lfo_mods[ii]->mod_amount * lfo_vals[ii];
                        break;

                    case eModDests::pwm:
                    default:
                        for (int jj = 0; jj < NUM_OSCS; ++jj)
                        {
                            m_oscs[jj]->setPWMod(m_lfo_mods[ii]->mod_amount * lfo_vals[ii]);
                        }
                        break;
                    }

                    lfopitch_mods[ii] = common::cclamp1f(lfopitch_mods[ii]);
                }
            }

            lfopitch_mod = common::cclamp1f(lfopitch_mod);

            // Portamento
            if (m_glide_type != eGlideTypes::off)
            {
                if (m_glide_increment > 0.0f && m_frequency != m_glide_target_frequency)
                {
                    if (m_glide_counter >= 1.0f)
                    {
                        m_frequency = m_glide_target_frequency;

                        if (m_glide_type == eGlideTypes::glissando)
                        {
                            m_glissando_frequency = m_frequency;
                        }
                        // done, reset
                        m_glide_counter = 0.0f;
                    }
                    else
                    {
                        m_frequency = m_glide_start_frequency
                            * getGlideMultiplier(m_glide_counter * getNotesBetweenFrequencies(m_glide_start_frequency, m_glide_target_frequency));
                        m_glide_counter += m_glide_increment;

                        if (m_glide_type == eGlideTypes::glissando)
                        {
                            m_glissando_frequency = note2Freq(getNoteClosestToFreq(m_frequency));
                        }
                    }
                }
                for (int jj = 0; jj < NUM_OSCS; ++jj)
                {
                    if (m_glide_type == eGlideTypes::glissando)
                    {
                        m_oscs[jj]->setPitch(m_glissando_frequency);
                    }
                    else
                    {
                        m_oscs[jj]->setPitch(m_frequency);
                    }
                }
            }

            float filter_keytrack_mod = m_filter_keytrack_offset_freq / (0.5f * M_SAMPLERATE);

            if (m_synth_type == eSynthTypes::substractive)
            {
                // Substractive synth mode
                float pitch_env_mod_mult = 1.0f;
                if (m_pitch_env_mod > 0.0f || m_pitch_env_mod < 0.0f)
                {
                    // when the pitch envelope is at 0.0f the pitch should be unaltered
                    pitch_env_mod_mult += (getScaledPitchEnvModMultiplier() * m_envs[k_pitch_env]->getSample());
                }

                for (ii = 0; ii < NUM_OSCS; ++ii)
                {
                    m_oscs[ii]->setPitchMod(lfopitch_mod + lfopitch_mods[ii]);
                    m_oscs[ii]->setPitch(m_frequency * pitch_env_mod_mult);
                    retVal += (1.0f + amp_mods[ii]) * m_oscs[ii]->getSample();
                }

                // Filter
                temp = 0.0f;
                float filter_env_val = m_envs[k_filter_env]->getSample();
                for (ii = 0; ii < NUM_FILTERS; ++ii)
                {
                    // velmod == -1 to 1
                    // envelope is 0 to 1
                    m_filters[ii]->setMod(m_filter_mods[ii]->env_mod + (2.0f * (m_filter_mods[ii]->vel_mod * m_velocity)), filter_env_val);
                    m_filters[ii]->setStaticMod(filter_keytrack_mod * 2.0f * m_filter_mods[ii]->keytrack + filter_mods[ii]);

                    if (m_filter_routing == eFilterRoutings::serial)
                    {
                        retVal = m_filters[ii]->process(retVal);
                    }
                }

                if (m_filter_routing == eFilterRoutings::parallel)
                {
                    temp = m_filters[0]->process(retVal);
                    retVal = 0.5f * (temp + m_filters[1]->process(retVal));
                }

                // Amp envelope has reached the end, setting voice inactive
                if (m_envs[k_amp_env]->isActive() == false)
                {
                    m_is_active = false;
                    for (ii = 0; ii < NUM_OSCS; ++ii)
                    {
                        m_oscs[ii]->stop();
                    }
                }

                // Scale according to velocity (if applicable) and return.
                retVal *= m_envs[k_amp_env]->getSample() * (1 + amp_mod);
                return (m_amp_env_vel_mod * m_velocity * retVal) + ((1.0f - m_amp_env_vel_mod) * retVal * 0.707f);
            }
            else
            {
                // FM synth mode

                // LFO pitch modulations
                for (ii = 0; ii < NUM_OSCS; ++ii)
                {
                    m_oscs[ii]->setPitchMod(lfopitch_mod + lfopitch_mods[ii]);
                    retVal += (1.0f + amp_mods[ii]) * m_oscs[ii]->getSample();
                }

                temp = 0.0f;
                switch (m_synth_type)
                {
                case eSynthTypes::fm_algo1:
                    retVal = m_envs[2]->getSample() * m_oscs[2]->getSample();
                    m_oscs[2]->setPhaseMod(m_osc3_feedback * retVal);
                    m_oscs[2]->setPitch(m_frequency * m_osc_ratios[2]);

                    m_oscs[1]->setPhaseMod(retVal);
                    m_oscs[1]->setPitch(m_frequency * m_osc_ratios[1]);
                    retVal = m_envs[1]->getSample() * m_oscs[1]->getSample();

                    m_oscs[0]->setPhaseMod(retVal);
                    m_oscs[0]->setPitch(m_frequency * m_osc_ratios[0]);
                    retVal = m_envs[0]->getSample() * m_oscs[0]->getSample();
                    break;

                case eSynthTypes::fm_algo2:
                    retVal = m_envs[2]->getSample() * m_oscs[2]->getSample();
                    m_oscs[2]->setPhaseMod(m_osc3_feedback * retVal);
                    m_oscs[2]->setPitch(m_frequency * m_osc_ratios[2]);

                    m_oscs[1]->setPitch(m_frequency * m_osc_ratios[1]);
                    retVal += m_envs[1]->getSample() * m_oscs[1]->getSample();

                    m_oscs[0]->setPhaseMod(retVal);
                    m_oscs[0]->setPitch(m_frequency * m_osc_ratios[0]);
                    retVal = m_envs[0]->getSample() * m_oscs[0]->getSample();
                    break;

                case eSynthTypes::fm_algo3:
                    m_oscs[1]->setPitch(m_frequency * m_osc_ratios[1]);
                    retVal = m_envs[1]->getSample() * m_oscs[1]->getSample();

                    m_oscs[0]->setPhaseMod(retVal);
                    m_oscs[0]->setPitch(m_frequency * m_osc_ratios[0]);
                    retVal = m_envs[0]->getSample() * m_oscs[0]->getSample();

                    temp = m_envs[2]->getSample() * m_oscs[2]->getSample();
                    m_oscs[2]->setPhaseMod(m_osc3_feedback * temp);
                    m_oscs[2]->setPitch(m_frequency * m_osc_ratios[2]);
                    retVal += temp;
                    break;

                case eSynthTypes::fm_algo4:
                    retVal = m_envs[2]->getSample() * m_oscs[2]->getSample();
                    m_oscs[2]->setPhaseMod(m_osc3_feedback * retVal);
                    m_oscs[2]->setPitch(m_frequency * m_osc_ratios[2]);

                    m_oscs[1]->setPhaseMod(retVal);
                    m_oscs[1]->setPitch(m_frequency * m_osc_ratios[1]);
                    retVal = m_envs[1]->getSample() * m_oscs[1]->getSample();

                    m_oscs[0]->setPitch(m_frequency * m_osc_ratios[0]);
                    retVal += m_envs[0]->getSample() * m_oscs[0]->getSample();
                    break;

                case eSynthTypes::fm_algo5:
                    retVal = m_envs[2]->getSample() * m_oscs[2]->getSample();
                    m_oscs[2]->setPhaseMod(m_osc3_feedback * retVal);
                    m_oscs[2]->setPitch(m_frequency * m_osc_ratios[2]);

                    m_oscs[1]->setPhaseMod(retVal);
                    m_oscs[0]->setPhaseMod(retVal);

                    m_oscs[1]->setPitch(m_frequency * m_osc_ratios[1]);
                    retVal += m_envs[1]->getSample() * m_oscs[1]->getSample();

                    m_oscs[0]->setPitch(m_frequency * m_osc_ratios[0]);
                    retVal += m_envs[0]->getSample() * m_oscs[0]->getSample();
                    break;

                case eSynthTypes::fm_algo6:
                default:
                    retVal = m_envs[2]->getSample() * m_oscs[2]->getSample();
                    m_oscs[2]->setPhaseMod(m_osc3_feedback * retVal);
                    m_oscs[2]->setPitch(m_frequency * m_osc_ratios[2]);

                    m_oscs[1]->setPitch(m_frequency * m_osc_ratios[1]);
                    retVal += m_envs[1]->getSample() * m_oscs[1]->getSample();

                    m_oscs[0]->setPitch(m_frequency * m_osc_ratios[0]);
                    retVal += m_envs[0]->getSample() * m_oscs[0]->getSample();
                    break;
                }

                // See if all envelopes have reached their ends and set voice inactive if they do
                m_is_active = false;
                for (ii = 0; ii < NUM_ENVS; ++ii)
                {
                    if (m_envs[ii]->isActive() == true)
                    {
                        m_is_active = true;
                        ii = NUM_ENVS;
                    }
                }

                if (m_is_active == false)
                {
                    for (ii = 0; ii < NUM_OSCS; ++ii)
                    {
                        m_oscs[ii]->stop();
                    }
                }

                // Filter
                temp = 0.0f;
                for (ii = 0; ii < NUM_FILTERS; ++ii)
                {
                    m_filters[ii]->setStaticMod((m_filter_mods[ii]->vel_mod * m_velocity) + filter_keytrack_mod * 2.0f * m_filter_mods[ii]->keytrack + filter_mods[ii]);

                    if (m_filter_routing == eFilterRoutings::serial)
                    {
                        retVal = m_filters[ii]->process(retVal);
                    }
                }

                if (m_filter_routing == eFilterRoutings::parallel)
                {
                    temp = m_filters[0]->process(retVal);
                    retVal = 0.5f * (temp + m_filters[1]->process(retVal));
                }

                return retVal * m_velocity;
            }
        }

    private:
        // The samplerate we're running at.
#if defined(OPS_GUI)
        float m_samplerate;

        bool m_soft_pedal_state;
        bool m_trigger_lfo3_on_pedal;
#endif

        // Synth type, includes different FM algorithms
        int m_synth_type;

        // The base frequency of the current note
        float m_frequency;

        // The current glissando frequency,
        // rounded up or down from the current internal frequency
        float m_glissando_frequency;

        // The current (MIDI) note value for the voice.
        int m_note;

        // The velocity of the current note.
        float m_velocity;

        // The current pitch bend value.
        // We store this because it won't necessarily be 0 when a noteOn message
        // is received, in which case we need to set the increment accordingly.
        float m_pitch_bend;

        // Whether or not the voice is currently active or not.
        // This is used so that PolyHandler can tell when a voice has finished
        // outputting sound, as it won't be when the noteOff() method is called
        // due to the release section still having to complete.
        bool m_is_active;

        // Whether or not the voice is in a note on state.
        // This differs from m_is_active in the way that m_is_active is true while
        // the voice is generating sound (amplitude envelope has not finished)
        // but m_note_on is only true until a noteOff() is called
        // and the envelopes enter the release state.
        bool m_note_on;

        // The envelope generators for amp, filter and pitch
#if defined(OPS_GUI)
        vector<shared_ptr<EnvGen>> m_envs;
        vector<shared_ptr<FilterMods>> m_filter_mods;
        vector<shared_ptr<LfoMods>> m_lfo_mods;
        vector<shared_ptr<Oscillator>> m_oscs;
        vector<shared_ptr<Oscillator>> m_lfos;
        vector<shared_ptr<Filter>> m_filters;
#else
        vector<unique_ptr<EnvGen>> m_envs;
        vector<unique_ptr<FilterMods>> m_filter_mods;
        vector<unique_ptr<LfoMods>> m_lfo_mods;
        vector<unique_ptr<Oscillator>> m_oscs;
        vector<unique_ptr<Oscillator>> m_lfos;
        vector<unique_ptr<Filter>> m_filters;
#endif

        float m_amp_env_vel_mod;

        float m_pitch_env_mod;

        float m_filter_keytrack_offset_freq;
        float m_lfo_keytrack_offset;

        vector<float> m_osc_ratios;

        int m_filter_routing;

        int m_glide_type;
        float m_glide_time;
        float m_glide_increment;
        float m_glide_counter;
        float m_glide_start_frequency;
        float m_glide_target_frequency;

        // Osc3 feedback for FM. Only osc3 has feedback.
        float m_osc3_feedback;
};

/// Voice unique pointer type.
using VoiceUptr = unique_ptr<Voice>;

#endif
