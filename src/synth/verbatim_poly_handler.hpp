#pragma once

#ifndef POLY_HANDLER_HPP
#define POLY_HANDLER_HPP

#include "verbatim_parameters.hpp"
#include "verbatim_common.hpp"
#include "verbatim_voice.hpp"
#include "ops_log.hpp"

#if defined(USE_VGL) && USE_VGL
#include "vgl/vgl_vector.hpp"
#include "vgl/vgl_unique_ptr.hpp"
using vgl::vector;
using vgl::unique_ptr;
#else
#include <vector>
using std::vector;
#include <memory>
using std::shared_ptr;
using std::unique_ptr;
#endif

/** Maximum number of voices to handle.
 *
 * Don't know if this should be dynamic, leaving like this for now.
 */
#ifndef NUM_VOICES
#define NUM_VOICES 16
#endif

/** \brief Polyphony handler class.
 *
 * Handles which notes get routed to which voices.
 *
 * Idea and parts of code from Niall Moody's Polyphonic Synth Tutorial at
 * http://www.niallmoody.com/ndcplugs/programming.htm
 *
 * NOTE:
 * This is essentially the interface class for the synth, all controls go via this.
 * As the roots of the synth lie in a VSTi plugin, the implementation is not as efficient
 * as one might expect from a synth used in a size-coded intro. Will probably be rewritten eventually.
 * The VSTi plugin is to be released eventually as well once all planned features are implemented.
 *
 * The synth itself is inspired by Waldorf MicroQ and some of the idiosyncracies are based on the way
 * MicroQ operates even though the implementation and feature sets differ.
 *
 */

class PolyHandler
{
    public:
        //----------------------------------------------------------------------------
        PolyHandler()
        {
            for (int ii = 0; ii < NUM_VOICES; ++ii)
            {
                m_voices.emplace_back(new Voice());
                m_notes.emplace_back();
            }
        }

        //----------------------------------------------------------------------------
        ~PolyHandler()
        {
        }

        //----------------------------------------------------------------------------
        /** \brief Fake constructor.
         *
         * @param params Parameters.
         * @param num_params Parameter count.
         */
        void init(int params[], int num_params)
        {
            int ii;

            // Setting all available notes/voices to inactive
            for (ii = 0; ii < NUM_VOICES; ++ii)
            {
                m_notes[ii] = -1;
            }

            // Setting all instrument parameters as defined in the input array
            for (ii = 0; ii < num_params; ++ii)
            {
                setParameter(ii, params[ii]);
            }
        }

#if defined(OPS_GUI)
        //----------------------------------------------------------------------------
        // A helper function for the GUI to know how many voices are available
        int getNumVoices()
        {
            return NUM_VOICES;
        }

        //----------------------------------------------------------------------------
        // A helper function for the GUI to know if a voice is active
        bool getIsVoiceActive(int voiceNumber)
        {
            return m_voices[voiceNumber]->getIsActive();
        }

        //----------------------------------------------------------------------------
        // A helper function for the GUI to know if a voice is fading out and can be reallocated
        bool getIsVoiceReleased(int voiceNumber)
        {
            return m_voices[voiceNumber]->getIsReleased();
        }
#endif

        //----------------------------------------------------------------------------
        // Called when the plugin receives a MIDI note on message.
#if defined(OPS_GUI)
        int noteOn(int note, float velocity)
#else
        void noteOn(int note, float velocity)
#endif
        {
            int ii = 0;
            bool done = false;

            if (m_poly_mode == ePolyModes::poly)
            {
                for (ii = 0; ii < NUM_VOICES; ++ii)
                {
                    // If the note is already assigned to a voice, trigger it again
                    if (m_notes[ii] == note)
                    {
                        m_voices[ii]->noteOn(note, m_prev_note, velocity);
                        done = true;
                        break;
                    }
                }

                if (!done)
                {
                    for (ii = 0; ii < NUM_VOICES; ++ii)
                    {
                        // If the current voice is not already playing, assign it the new note.
                        if (!m_voices[ii]->getIsActive())
                        {
                            m_voices[ii]->noteOn(note, m_prev_note, velocity);
                            m_notes[ii] = note;
                            done = true;
                            ++m_num_active_notes;
                            break;
                        }
                    }
                }

                if (!done)
                {
                    // No available notes, need to steal
                    //TODO: implement heuristics for sensible stealing
                    for (ii = 0; ii < NUM_VOICES; ++ii)
                    {
                        // Steal if the currently playing note is lower than the new one
                        if (m_voices[ii]->getCurrentNote() < note)
                        {
                            m_voices[ii]->noteOn(note, m_prev_note, velocity);
                            m_notes[ii] = note;
                            break;
                        }
                    }
                    // Still nothing, steal the first note
                    m_voices[0]->noteOn(note, m_prev_note, velocity);
                    m_notes[0] = note;
                }
            }
            else // mono/legato
            {
                if (m_poly_mode == ePolyModes::legato)
                {
                    if (m_num_active_notes == 0 && (m_glide_type != eGlideTypes::off))
                    {
                        m_prev_note = -1;
                    }
                    // TODO: see if something else needs to be done differently for legato mode
                }

                if (m_num_active_notes == 0 || m_poly_mode == ePolyModes::mono)
                {
                    m_voices[0]->noteOn(note, m_prev_note, velocity);
                }
                else
                {
                    m_voices[0]->noteOn(note, m_prev_note, -1.0f);
                }

                ++m_newest_note_index;
                if (m_newest_note_index < NUM_VOICES)
                {
                    ++m_num_active_notes;
                }
                else
                {
                    // TODO: Not sure if this makes sense. Check if we should just ignore notes past NUM_VOICES right away.
                    m_newest_note_index = 0;
                }
                m_notes[m_newest_note_index] = note;
            }

            m_prev_note = note;

#if defined(OPS_GUI)
            if (m_poly_mode == ePolyModes::poly)
            {
                return ii;
            }
            else
            {
                return 0;
            }
#endif
        }

        //----------------------------------------------------------------------------
        // Called when the plugin receives a MIDI note off message.
        // Finds the voice associated with this note, and calls noteOff()
        // for it. Depending on the length of the voice's
        // envelope's release section, it won't stop outputting sound
        // immediately. We need to check each active voice in getSample() to
        // determine whether or not it's still active.
        void noteOff(int note)
        {
            int ii;

            --m_num_active_notes;
            if (m_num_active_notes < 0)
            {
                m_num_active_notes = 0;
            }

            if (m_poly_mode == ePolyModes::poly)
            {
                for (ii = 0; ii < NUM_VOICES; ++ii)
                {
                    if (m_notes[ii] == note)
                    {
                        m_voices[ii]->noteOff(false);

                        // Note that voices[i] will still be active here (envelope release).
                        m_notes[ii] = -1;
                        break;
                    }
                }
            }
            else // mono/legato
            {
                // In monophonic mode we should not send a real note off event to a voice until no keys are held.
                // Instead, if the released note is the one currently playing, we change the pitch of the voice
                // to that of one of the previous notes still being held.
                if (m_num_active_notes > 0)
                {
#if defined(USE_LD)
                    assert(m_newest_note_index < NUM_VOICES);
#endif
                    if (note == m_notes[m_newest_note_index])
                    {
                        m_notes[m_newest_note_index] = -1;
                        for (ii = m_newest_note_index; ii >= 0; --ii)
                        {
                            if (m_notes[ii] != -1)
                            {
                                m_newest_note_index = ii;
                                break;
                            }
                        }
                        m_voices[0]->noteOn(m_notes[m_newest_note_index], note, -1.0f);
                        m_prev_note = m_notes[m_newest_note_index];
                    }
                    else
                    {
                        for (ii = m_newest_note_index; ii >= 0; --ii)
                        {
                            if (m_notes[ii] == note)
                            {
                                m_notes[ii] = -1;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    m_voices[0]->noteOff(false);
                    m_newest_note_index = -1;
                }
            }
        }

        //----------------------------------------------------------------------------
        void allNotesOff()
        {
            for (int ii = 0; ii < NUM_VOICES; ++ii)
            {
                m_voices[ii]->noteOff(true);
                m_notes[ii] = -1;
                // Note that the voices will still be active here (envelope release).
            }
            m_num_active_notes = 0;
            m_newest_note_index = -1;
        }

        //----------------------------------------------------------------------------
        bool getIsActive()
        {
            if (m_num_active_notes > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        //----------------------------------------------------------------------------
        void setParameter(int parameter, int value)
        {
            setParameter(parameter, static_cast<float>(value) / 65535.0f);
        }

        //----------------------------------------------------------------------------
        // Called when the plugin's setParameter() method is called.
        // Passes all parameters not handled here to the voices.
        void setParameter(int parameter, float value)
        {
            // Handle channel volume and panning related parameters here and pass the rest to each voice for handling
            switch (parameter)
            {
            case eSSynth::k_pan:
#if defined(OPS_GUI)
                if (value < 0.0f || value > 1.0f)
                {
                    std::ostringstream sstr;
                    sstr << "Error: PolyHandler::setParameter Param: " << parameter << " Value out of bounds: " << value << std::endl;
                    throw std::runtime_error(sstr.str());
                }
#endif
                m_pan = value;
                break;

            case eSSynth::k_poly_mode:
                setPolyMode(value);
                break;

            case eSSynth::k_glide_type:
                setGlideType(value);
                break;

            case eSSynth::k_samplerate:
            case eSSynth::k_tempo:
                break;

                // Normalize parameter values to 0-1 range
            default:
#if defined(OPS_GUI)
                if (value < 0.0f || value > 1.0f)
                {
                    std::ostringstream sstr;
                    sstr << "Error: PolyHandler::setParameter Param: " << parameter << " Value out of bounds: " << value << std::endl;
                    throw std::runtime_error(sstr.str());
                }
#endif
                value = common::cclampf(value, 0.0f, 1.0f);
                break;
            }
            // Feed through to all voices
            for (int ii = 0; ii < NUM_VOICES; ++ii)
            {
                m_voices[ii]->setParameter(parameter, value);
            }
        }

        //----------------------------------------------------------------------------
        void setPolyMode(float value)
        {
#if defined(OPS_GUI)
            if (value < 0.0f || value > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Error: PolyHandler::setPolyMode: Value out of bounds: " << value << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif

            // Reset notes when polyphony mode changes
            allNotesOff();

            m_poly_mode = common::clrintf(value * static_cast<float>(ePolyModes::num_items - 1));
        }

        //----------------------------------------------------------------------------
        void setGlideType(float value)
        {
#if defined(OPS_GUI)
            if (value < 0.0f || value > 1.0f)
            {
                std::ostringstream sstr;
                sstr << "Error: PolyHandler::setGlideType: Value out of bounds: " << value << std::endl;
                throw std::runtime_error(sstr.str());
            }
#endif
            m_glide_type = common::clrintf(value * static_cast<float>(eGlideTypes::num_items - 1));
        }

#if defined(OPS_GUI)
        //----------------------------------------------------------------------------
        void setSoftPedalState(bool soft_pedal_state)
        {
            for (int ii = 0; ii < NUM_VOICES; ++ii)
            {
                m_voices[ii]->setSoftPedalState(soft_pedal_state);
            }
        }
#endif

        //----------------------------------------------------------------------------
        // Called for every sample in the plugin's process/processReplacing loop.
        // Calls all active voices, mixes and pans the collected samples
        // and returns the mixed sample.
        void getSample(float &leftsample, float &rightsample)
        {
            float retval = 0.0f;

            for (int ii = 0; ii < NUM_VOICES; ++ii)
            {
                //Only get samples for active voices.
                if (m_voices[ii]->getIsActive())
                {
                    retval += m_voices[ii]->getSample();
                }
            }

            // Handle channel panning and process delay
            leftsample = retval * ops_sqrtf(1.0f - m_pan);
            rightsample = retval * ops_sqrtf(m_pan);
        }

    private:

        /// Array of voices.
        vector<VoiceUptr> m_voices;

        int m_poly_mode = ePolyModes::poly;
        int m_num_active_notes = 0;

        /// Holds the currently-assigned note values for each voice.
        // Note: in monophonic mode each value corresponds to a key held
        // and the latest note index should match m_num_active_notes
        vector<int> m_notes;
        int m_newest_note_index = -1;

        int m_glide_type = eGlideTypes::off;

        // Last played note, used for portamento.
        // A negative value indicates that the next note should be assigned as is without glide.
        int m_prev_note = -1;

        float m_pan = 0.5f;
};

#endif
