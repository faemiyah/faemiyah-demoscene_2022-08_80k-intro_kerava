#pragma once
/** \file File used to include all synth code verbatim.
*
* To be used when compiling into actual, small binary version.
*
*/

#define OPS_CLAMP_OUT 0
#ifndef AUDIO_SAMPLERATE
#define AUDIO_SAMPLERATE 44100
#endif

#define USE_VGL 1

#if defined(USE_VGL) && USE_VGL
#include "vgl/vgl_array.hpp"
#include "vgl/vgl_unique_ptr.hpp"
#include "vgl/vgl_vector.hpp"
using vgl::array;
using vgl::unique_ptr;
using vgl::vector;
#else
#include <array>
#include <memory>
#include <vector>
using std::array;
using std::shared_ptr;
using std::vector;
#endif

vector<float> *g_sample_buffers;

#define NUM_VOICES 8

// Song, instrument, FX and routing data + related generated synth macros
#include "songdata.hpp"

#ifndef SONGDATA_HPP
#error "Missing song data."
#endif

#if defined(TEST_EXECUTION)
#include <fstream>

namespace
{
    // Convert signed int8 to a float sample
    float toSampleFloat(char c)
    {
        return ((static_cast<float>(c) + 128.0f) / 255.0f) - 0.5f;
    }
}
#endif

#include "verbatim_common.hpp"
#include "verbatim_parameters.hpp"
#if NUM_ECHO_TRACKS > 0
#include "verbatim_echo.hpp"
#endif
#include "verbatim_env_gen.hpp"
#include "verbatim_filter.hpp"
#if NUM_DISTORTION_TRACKS > 0
#include "verbatim_distortion.hpp"
#endif
#include "verbatim_oscillator.hpp"
#include "verbatim_poly_handler.hpp"
#if NUM_CHORUS_TRACKS > 0
#include "verbatim_chorus.hpp"
#endif
#if NUM_REVERB_TRACKS > 0
#include "verbatim_reverb.hpp"
#endif
#if NUM_FILTER_TRACKS > 0
#include "verbatim_stereo_filter.hpp"
#endif
// Always include voices as not having a sound source would make no sense.
#include "verbatim_voice.hpp"

#if defined(WIN32)
#include <cstdio>
#include <iostream>
#endif

#if defined(TEST_EXECUTION)
void generate_audio(float* audio_buffer, unsigned buffer_length, vector<float>* sample_buffers, int sample_count, float& progress)
#else
void generate_audio(float *audio_buffer, unsigned buffer_length, vector<float> *sample_buffers, float &progress)
#endif
{
#if USE_VGL
    g_sample_buffers = sample_buffers;
#else
    g_sample_buffers = new vector<float>[9];
    for (int ii = 0; ii < 9; ++ii)
    {
        for (uint32_t jj = 0; jj < sample_buffers[ii].size(); ++jj)
        {
            g_sample_buffers[ii].emplace_back(sample_buffers[ii][jj]);
        }
    }
#endif
    progress = 0.0f;

#if defined(DNLOAD_USE_LD)
    std::cout << "Start audio generation.\n";
    std::cout << "Buffer length: " << buffer_length << "\n";
#endif
    float *out = audio_buffer;
    uint32_t i;
    uint32_t event_index;
    int next_event_timestamp = 0;
    int32_t k;
    int32_t l;
    int32_t events_left;
    int32_t total_events_left;
#if defined(DNLOAD_USE_LD)
    uint16_t idle_countdown = 0;
#endif

    vector<unique_ptr<PolyHandler>> instr_tracks;
    for (unsigned ii = 0; (ii < NUM_INSTR_TRACKS); ++ii)
    {
        instr_tracks.emplace_back(new PolyHandler());
        instr_tracks[ii]->init(instr_params[ii], eSSynth::k_num_user_params);
    }

#if NUM_CHORUS_TRACKS > 0
    vector<unique_ptr<Chorus>> chorus_tracks;
    for (unsigned ii = 0; (ii < NUM_CHORUS_TRACKS); ++ii)
    {
        chorus_tracks.emplace_back(new Chorus());
        chorus_tracks[ii]->init(chorus_params[ii], eChorus::k_num_user_params);
    }
#endif

#if NUM_ECHO_TRACKS > 0
    vector<unique_ptr<Echo>> echo_tracks;
    for (unsigned ii = 0; (ii < NUM_ECHO_TRACKS); ++ii)
    {
        echo_tracks.emplace_back(new Echo());
        echo_tracks[ii]->init(echo_params[ii], eEcho::k_num_user_params);
    }
#endif

#if NUM_REVERB_TRACKS > 0
    vector<unique_ptr<Reverb>> reverb_tracks;
    for (unsigned ii = 0; (ii < NUM_REVERB_TRACKS); ++ii)
    {
        reverb_tracks.emplace_back(new Reverb());
        reverb_tracks[ii]->init(reverb_params[ii], eReverb::k_num_user_params);
    }
#endif

#if NUM_DISTORTION_TRACKS > 0
    vector<unique_ptr<Distortion>> distortion_tracks;
    for (unsigned ii = 0; (ii < NUM_DISTORTION_TRACKS); ++ii)
    {
        distortion_tracks.emplace_back(new Distortion());
        distortion_tracks[ii]->init(distortion_params[ii], eDist::k_num_user_params);
    }
#endif

#if NUM_FILTER_TRACKS > 0
    vector<unique_ptr<StereoFilter>> filter_tracks;
    for (unsigned ii = 0; (ii < NUM_FILTER_TRACKS); ++ii)
    {
        filter_tracks.emplace_back(new StereoFilter());
        filter_tracks[ii]->init(filter_params[ii], eStereoFilter::k_num_user_params);
    }
#endif

    float track_outs[NUM_TRACK_OUTPUTS];
    float track_volume_multipliers[NUM_TRACKS];
    for (unsigned ii = 0; (ii < NUM_TRACKS); ++ii)
    {
        track_volume_multipliers[ii] = 1.0f;
    }

#if defined(HAS_DIVISION_EVENTS)
#define DIVISION division
    float division = GLOBAL_DIVISIONF;
#else
#define DIVISION GLOBAL_DIVISIONF
#endif

#if defined(HAS_TEMPO_EVENTS)
    float tempo_in_bpm = GLOBAL_TEMPOF;
#endif

    float tempo_in_microseconds_per_quarternote = GLOBAL_TEMPO_IN_MICROSECS_PQNF;
    float srtick = ((SAMPLERATE / 1000000.0f) * (tempo_in_microseconds_per_quarternote / DIVISION));

    event_index = 0;
    total_events_left = ((sizeof(g_song_data) / sizeof(*g_song_data)) / 5);
#if defined(DNLOAD_USE_LD)
    std::cout << "Processing " << total_events_left << " events.\n";
#endif

    if ((event_index * 5) < (sizeof(g_song_data) / sizeof(*g_song_data)))
    {
        next_event_timestamp = common::clrintf(static_cast<float>(g_song_data[event_index * 5]) * srtick);
    }
    for (i = 0; (i < (buffer_length / sizeof(float) / 2)); ++i)
    {
        // Process control data
        if ((total_events_left > 0) && (i == static_cast<uint32_t>(next_event_timestamp)))
        {
            events_left = 1;
            while (events_left > 0)
            {
                k = static_cast<int>(event_index * 5);
                int eventnum = static_cast<int>(g_song_data[k + 1]);
                unsigned event_channel_num = g_song_data[k + 2];
                int event_param_1 = static_cast<int>(g_song_data[k + 3]);
                int event_param_2 = static_cast<int>(g_song_data[k + 4]);
                switch (eventnum)
                {
                case synth_event_types::NoteOn:
                    if (event_param_2 > 0)
                    {
                        instr_tracks[event_channel_num]->noteOn(event_param_1, static_cast<float>(event_param_2) / 127.0f);
                    }
                    else
                    {
                        instr_tracks[event_channel_num]->noteOff(event_param_1);
                    }
                    break;

#ifdef HAS_NOTE_OFF_EVENTS
                case synth_event_types::NoteOff:
                    instr_tracks[event_channel_num]->noteOff(event_param_1);
                    break;
#endif

#ifdef HAS_PITCHBEND_EVENTS
                case synth_event_types::PitchBend:
                    instr_tracks[event_channel_num]->setParameter(eSSynth::k_pitchbend, static_cast<float>(event_param_2));
                    break;
#endif

#ifdef HAS_NRPN_EVENTS
                case synth_event_types::NRPN:
                    for (l = 0; l < NUM_AUTOMATION_ENVELOPES; ++l)
                    {
                        if (param_env_states[l].m_track_num == event_channel_num && param_env_states[l].m_param_id == event_param_1)
                        {
                            param_env_states[l].m_param_value = static_cast<float>(event_param_2) / 65535.0f;
                            if (param_env_states[l].m_param_id == VOLUME_ENVELOPE_CONTROL_NUMBER)
                            {
                                track_volume_multipliers[event_channel_num] = param_env_states[l].m_param_value;
                            }
                        }
                    }

                    if (event_channel_num < NUM_INSTR_TRACKS)
                    {
                        instr_tracks[event_channel_num]->setParameter(event_param_1, event_param_2);
                        break;
                    }
#if NUM_DISTORTION_TRACKS > 0
                    // Distortion tracks
                    if (event_channel_num < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS)
                    {
                        distortion_tracks[event_channel_num - (NUM_INSTR_TRACKS)]->setParameter(event_param_1, event_param_2);
                        break;
                    }
#endif
#if NUM_CHORUS_TRACKS > 0
                    // Chorus tracks
                    if (event_channel_num < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS)
                    {
                        chorus_tracks[event_channel_num - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS)]->setParameter(event_param_1, event_param_2);
                        break;
                    }
#endif
#if NUM_ECHO_TRACKS > 0
                    // Echo tracks
                    if (event_channel_num < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS)
                    {
                        echo_tracks[event_channel_num - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS)]->setParameter(event_param_1, event_param_2);
                        break;
                    }
#endif
#if NUM_REVERB_TRACKS > 0
                    // Reverb tracks
                    if (event_channel_num < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS + NUM_REVERB_TRACKS)
                    {
                        reverb_tracks[event_channel_num - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS)]->setParameter(event_param_1, event_param_2);
                        break;
                    }
#endif
#if NUM_FILTER_TRACKS > 0
                    // Stereo filter tracks
                    if (event_channel_num < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS + NUM_REVERB_TRACKS + NUM_FILTER_TRACKS)
                    {
                        filter_tracks[event_channel_num - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS + NUM_REVERB_TRACKS)]->setParameter(event_param_1, event_param_2);
                        break;
                    }
#endif
                    break;
#endif

#ifdef HAS_DIVISION_EVENTS
                case synth_event_types::Division:
                    division = static_cast<float>(event_param_2);
                    srtick = ((SAMPLERATE / 1000000.0f) * (tempo_in_microseconds_per_quarternote / DIVISION));
                    break;
#endif

#ifdef HAS_TEMPO_EVENTS
                case synth_event_types::Tempo:
                    tempo_in_bpm = static_cast<float>(event_param_2) / TEMPO_INT_TO_FLOAT_DENOMINATOR;
                    tempo_in_microseconds_per_quarternote = 60000000.0f / tempo_in_bpm;
                    srtick = ((SAMPLERATE / 1000000.0f) * (tempo_in_microseconds_per_quarternote / DIVISION));

                    for (uint8_t t = 0; t < NUM_INSTR_TRACKS; ++t)
                    {
                        instr_tracks[t]->setParameter(eSSynth::k_tempo, tempo_in_bpm);
                    }

#if NUM_ECHO_TRACKS > 0
                    for (uint8_t t = 0; t < NUM_ECHO_TRACKS; ++t)
                    {
                        echo_tracks[t]->setParameter(eEcho::k_tempo, tempo_in_bpm);
                    }
#endif
                    break;
#endif

#ifdef HAS_ALL_NOTES_OFF_EVENTS
                case synth_event_types::AllNotesOff:
                    // TODO: handle or filter events out altogether during generation
#if defined(DNLOAD_USE_LD)
                    std::cout << "End of events.\n";
                    idle_countdown = AUDIO_SAMPLERATE;
#endif
                    break;
#endif

#ifdef HAS_ENVELOPE_EVENTS
                case synth_event_types::StartEnvelope:
                    for (l = 0; l < NUM_AUTOMATION_ENVELOPES; ++l)
                    {
                        if (param_env_states[l].m_track_num == event_channel_num && param_env_states[l].m_param_id == event_param_1 % 256)
                        {
                            param_env_states[l].m_target_param_value = (static_cast<float>(event_param_2) / 65535.0f);
                            param_env_states[l].m_samples_left = common::clrintf(static_cast<float>(event_param_1 / 256) * srtick);
                            param_env_states[l].m_value_to_add = (param_env_states[l].m_target_param_value - param_env_states[l].m_param_value)
                                / static_cast<float>(param_env_states[l].m_samples_left)
                                * ENVELOPE_INTERVALF;
                        }
                    }
                    break;
#endif

                default:
#if defined(DNLOAD_USE_LD)
                    printf("WARNING: undefined event: %d, %d, %d, %d, %d\n", g_song_data[k], eventnum, event_channel_num,
                        event_param_1, event_param_2);
#endif
                    break;
                }
                ++event_index;
                --events_left;
                --total_events_left;
                if ((event_index * 5) < (sizeof(g_song_data) / sizeof(*g_song_data)))
                {
                    next_event_timestamp += common::clrintf(static_cast<float>(g_song_data[(event_index * 5)]) * srtick);
                    if (g_song_data[(event_index * 5)] == 0)
                    {
                        ++events_left;
                    }
                }
            }
        }

        // Start processing audio and automation for instrument tracks
        for (size_t track_out_num = 0; track_out_num < NUM_TRACK_OUTPUTS; ++track_out_num)
        {
            track_outs[track_out_num] = 0.0f;
        }

        // TODO: add granularity to parameter changes so that parameter updates are not necessarily called every sample
        for (k = 0; k < NUM_TRACKS; ++k)
        {
#if defined(HAS_ENVELOPE_EVENTS)
            for (l = 0; l < NUM_AUTOMATION_ENVELOPES; ++l)
            {
                if (param_env_states[l].m_track_num == k && param_env_states[l].m_samples_left > 0)
                {
                    --param_env_states[l].m_samples_left;
                    --param_env_states[l].m_samples_to_next;
                    if (param_env_states[l].m_samples_to_next < 1)
                    {
                        param_env_states[l].m_param_value += param_env_states[l].m_value_to_add;

                        if (param_env_states[l].m_samples_left < ENVELOPE_INTERVAL)
                        {
                            param_env_states[l].m_samples_to_next = static_cast<int16_t>(param_env_states[l].m_samples_left);
                        }
                        else
                        {
                            param_env_states[l].m_samples_to_next = ENVELOPE_INTERVAL;
                        }

                        if ((param_env_states[l].m_value_to_add <= 0.0f) && (param_env_states[l].m_param_value < param_env_states[l].m_target_param_value))
                        {
                            param_env_states[l].m_param_value = param_env_states[l].m_target_param_value;
                        }
                        else if ((param_env_states[l].m_value_to_add > 0.0f) && (param_env_states[l].m_param_value > param_env_states[l].m_target_param_value))
                        {
                            param_env_states[l].m_param_value = param_env_states[l].m_target_param_value;
                        }

                        if (param_env_states[l].m_param_id == VOLUME_ENVELOPE_CONTROL_NUMBER)
                        {
                            track_volume_multipliers[k] = param_env_states[l].m_param_value;
                        }

                        if (k < NUM_INSTR_TRACKS)
                        {
                            instr_tracks[k]->setParameter(param_env_states[l].m_param_id, param_env_states[l].m_param_value);
                        }
                        else
                        {
#if NUM_FILTER_TRACKS > 0
                            if (k >= FIRST_FILTER_IDX)
                            {
                                filter_tracks[k - FIRST_FILTER_IDX]->setParameter(param_env_states[l].m_param_id, param_env_states[l].m_param_value);
                                continue;
                            }
#endif
#if NUM_REVERB_TRACKS > 0
                            if (k >= FIRST_REVERB_IDX)
                            {
                                reverb_tracks[k - FIRST_REVERB_IDX]->setParameter(param_env_states[l].m_param_id, param_env_states[l].m_param_value);
                                continue;
                            }
#endif
#if NUM_ECHO_TRACKS > 0
                            if (k >= FIRST_ECHO_IDX)
                            {
                                echo_tracks[k - FIRST_ECHO_IDX]->setParameter(param_env_states[l].m_param_id, param_env_states[l].m_param_value);
                                continue;
                            }
#endif
#if NUM_CHORUS_TRACKS > 0
                            if (k >= FIRST_CHORUS_IDX)
                            {
                                chorus_tracks[k - FIRST_CHORUS_IDX]->setParameter(param_env_states[l].m_param_id, param_env_states[l].m_param_value);
                                continue;
                            }
#endif
#if NUM_DISTORTION_TRACKS > 0
                            if (k >= FIRST_DISTORTION_IDX)
                            {
                                distortion_tracks[k - FIRST_DISTORTION_IDX]->setParameter(param_env_states[l].m_param_id, param_env_states[l].m_param_value);
                                continue;
                            }
#endif
                        }
                    }
                }
            }
#endif

            // Process and get samples

            if (k < NUM_INSTR_TRACKS)
            {
                instr_tracks[k]->getSample(track_outs[(k * 2)], track_outs[(k * 2) + 1]);
            }
            else
            {
                // VOI POJAT
#if NUM_DISTORTION_TRACKS > 0
                // Distortion tracks
                if (k < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS)
                {
                    distortion_tracks[k - (NUM_INSTR_TRACKS)]->process(&track_outs[(k * 2)], &track_outs[(k * 2)]);
                    goto track_volume_adjustment_section;
                }
#endif
#if NUM_CHORUS_TRACKS > 0
                // Chorus tracks
                if (k < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS)
                {
                    chorus_tracks[k - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS)]->process(&track_outs[(k * 2)], &track_outs[(k * 2)]);
                    goto track_volume_adjustment_section;
                }
#endif
#if NUM_ECHO_TRACKS > 0
                // Echo tracks
                if (k < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS)
                {
                    echo_tracks[k - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS)]->process(&track_outs[(k * 2)], &track_outs[(k * 2)]);
                    goto track_volume_adjustment_section;
                }
#endif
#if NUM_REVERB_TRACKS > 0
                // Reverb tracks
                if (k < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS + NUM_REVERB_TRACKS)
                {
                    reverb_tracks[k - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS)]->process(&track_outs[(k * 2)], &track_outs[(k * 2)]);
                    goto track_volume_adjustment_section;
                }
#endif
#if NUM_FILTER_TRACKS > 0
                // Stereo filter tracks
                if (k < NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS + NUM_REVERB_TRACKS + NUM_FILTER_TRACKS)
                {
                    filter_tracks[k - (NUM_INSTR_TRACKS + NUM_DISTORTION_TRACKS + NUM_CHORUS_TRACKS + NUM_ECHO_TRACKS + NUM_REVERB_TRACKS)]->process(&track_outs[(k * 2)], &track_outs[(k * 2)]);
                    goto track_volume_adjustment_section;
                }
#endif
            }
            track_volume_adjustment_section:
            track_outs[k * 2] *= track_volume_multipliers[k];
            track_outs[(k * 2) + 1] *= track_volume_multipliers[k];

            // Combine and route outputs to appropriate indices to be used for FX inputs
            for (int ll = 0; ll < NUM_ROUTING_ITEMS; ++ll)
            {
                if (track_routing[ll][1] == k)
                {
                    float level = static_cast<float>(track_routing[ll][2]) / 16384.0f;
                    int target_track = track_routing[ll][0];
                    track_outs[(target_track * 2)] += track_outs[(k * 2)] * level;
                    track_outs[(target_track * 2) + 1] += track_outs[(k * 2) + 1] * level;
                }
            }
        }

#if defined(OUTPUT_BUS_LEVEL)
        track_outs[LEFT_OUT_IDX] *= OUTPUT_BUS_LEVEL;
        track_outs[RIGHT_OUT_IDX] *= OUTPUT_BUS_LEVEL;
#endif

#if 0
        {
            static float out_max = -FLT_MAX;
            static float out_min = FLT_MAX;
#if defined(WIN32)
            float new_max = common::cfmaxf(common::cfmaxf(fabsf(track_outs[LEFT_OUT_IDX]), fabsf(track_outs[RIGHT_OUT_IDX])), out_max);
            float new_min = -common::cfminf(common::cfminf(fabsf(track_outs[LEFT_OUT_IDX]), fabsf(track_outs[RIGHT_OUT_IDX])), out_min);
#else
            float new_max = fmaxf(fmaxf(fabsf(track_outs[LEFT_OUT_IDX]), fabsf(track_outs[RIGHT_OUT_IDX])), out_max);
            float new_min = -fminf(fminf(fabsf(track_outs[LEFT_OUT_IDX]), fabsf(track_outs[RIGHT_OUT_IDX])), out_min);
#endif
            if (new_max > out_max)
            {
                printf("new output maximum; %1.3f\n", new_max);
                out_max = new_max;
            }
            if (new_min < out_min)
            {
                printf("new output minimum; %1.3f\n", new_min);
                out_min = new_min;
            }
        }
#endif

#if OPS_CLAMP_OUT
#if defined(APPROXIMATE_TANH)
        track_outs[LEFT_OUT_IDX] = common::rational_tanh(track_outs[LEFT_OUT_IDX]);
        track_outs[RIGHT_OUT_IDX] = common::rational_tanh(track_outs[RIGHT_OUT_IDX]);
#else
#if defined(WIN32)
        track_outs[LEFT_OUT_IDX] = tanhf(track_outs[LEFT_OUT_IDX]);
        track_outs[RIGHT_OUT_IDX] = tanhf(track_outs[RIGHT_OUT_IDX]);
#else
        track_outs[LEFT_OUT_IDX] = dnload_tanhf(track_outs[LEFT_OUT_IDX]);
        track_outs[RIGHT_OUT_IDX] = dnload_tanhf(track_outs[RIGHT_OUT_IDX]);
#endif
#endif
#endif

        out[i * 2] = track_outs[LEFT_OUT_IDX]; //left channel;
        out[i * 2 + 1] = track_outs[RIGHT_OUT_IDX]; //right channel

#if defined(DNLOAD_USE_LD)
        if ((i % ((buffer_length / sizeof(float) / 2) / 100)) == 0)
        {
            unsigned blen = static_cast<unsigned>(buffer_length / sizeof(float) / 2);
            progress = static_cast<float>(i) / static_cast<float>(blen);
            printf("|sample(%02.2f): %d / %u\n", progress, i, blen);
        }

        if (total_events_left <= 0)
        {
            if (track_outs[LEFT_OUT_IDX] > 0.01f)
            {
                idle_countdown = AUDIO_SAMPLERATE;
            }
            --idle_countdown;
            if (idle_countdown == 0)
            {
                std::cout << "Idle for " << AUDIO_SAMPLERATE << " samples, considering generation finished.\n";
                break;
            }
        }

#endif
    }
#if defined(DNLOAD_USE_LD)
    std::cout << "End audio generation.\n";
#endif
    progress = 1.0f;
}

