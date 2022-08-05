#pragma once

#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#if defined(OPS_GUI)
#define BETTER_ENUMS_MACRO_FILE "enum_macros.h"
#include "better_enums/enum.h"
#endif

#define g_lfo_max_frequency 250.0f // Hz
#define g_max_glide_time 5000.0 // milliseconds

#define g_max_fm_ratio 20.0f

// The following macros are needed for using the same definitions in both the VST plugins as well as the plain core versions

#define SINGLE_ARG(...) __VA_ARGS__

#if defined(OPS_GUI)
#define CONSTRUCT_ENUM(name, enumlist) \
BETTER_ENUM(name, int, \
	enumlist \
	)
#else
#define CONSTRUCT_ENUM(name, enumlist) \
namespace name \
{ \
	enum params \
		{ \
		enumlist \
		}; \
}
#endif

enum EParams
{
	kVolume = 0,
	kVelocity,
	kNumParams
};

// Global VST defines
#if defined(OPS_GUI)
#define g_num_programs 1
#endif
#define g_num_outputs 2;
#define g_pitch_env_range 24 // pitch envelope range in semitones
#define g_osc_semi_range 60 // oscillator tuning range in semitones [-g_osc_semi_range,g_osc_semi_range]
#define g_env_length_stage_max 10.0f // maximum length for an envelope stage in seconds
#define g_num_bpm_synced_lfo_speeds 30 // number of available bpm synced lfo speeds
#define g_delay_buffer_size 88200 // delay buffer size in seconds * samplerate
#define g_pitch_bend_range 2 // pitch bend range in notes +/-

#if defined(OPS_TYNNYRI)
CONSTRUCT_ENUM(eReverb, SINGLE_ARG(
	// Reverb params
	k_reverb_mix = 0,
	k_reverb_predelay,
	k_reverb_bandwidth, // lpf1 / input filtering after predelay (diffusion) (0.0-1.0, 0.9995)
	k_reverb_damping, // lpf2&lpf3 (0.0 - 1.0, 0.0005)
	k_reverb_decay, // g5 (0.0-1.0, 0.5)
	k_reverb_roomsize,

	k_num_user_params, // number of "real" fx params (all params minus gui junk)

	// additional gui params
	k_icaptioncontrol,
	k_fileopen,
	k_filesave,
	k_reset,
	k_panic,

	// end of parameter tags, k_num_params sets the number of available controls for the host UI
	k_num_params,

	// invisible parameters
	k_samplerate
))
#endif

#if defined(OPS_PAPUKAIJA)
CONSTRUCT_ENUM(eEcho, SINGLE_ARG(
// Delay/echo params
	k_delay_mode = 0,
	k_delay_mix,
	k_delay_time,
	k_delay_feedback,
	k_delay_lowpass_cutoff,
	k_delay_highpass_cutoff,

	k_num_user_params, // number of "real" fx params (all params minus gui junk)

	// additional gui params
	k_icaptioncontrol,
	k_fileopen,
	k_filesave,
	k_reset,
	k_panic,

	// end of parameter tags, k_num_params sets the number of available controls for the host UI
	k_num_params,

	// invisible parameters
	k_samplerate,
	k_tempo
	))
#endif

#if defined(OPS_KUORO)
CONSTRUCT_ENUM(eChorus, SINGLE_ARG(
// Delay/echo params
	k_chorus_mix = 0,
	k_chorus_delay,
	k_chorus_depth,
	k_chorus_rate,
	k_chorus_feedback,

	k_num_user_params, // number of "real" fx params (all params minus gui junk)

	// additional gui params
	k_icaptioncontrol,
	k_fileopen,
	k_filesave,
	k_reset,
	k_panic,

	// end of parameter tags, k_num_params sets the number of available controls for the host UI
	k_num_params,

	// invisible parameters
	k_samplerate
))
#endif

#if defined(OPS_MURTO)
CONSTRUCT_ENUM(eDist, SINGLE_ARG(
	// Delay/echo params
	k_dist_mode = 0,
	k_dist_mix,
	k_dist_drive,
	k_dist_post_gain,

	k_num_user_params, // number of "real" fx params (all params minus gui junk)

										 // additional gui params
	k_icaptioncontrol,
	k_fileopen,
	k_filesave,
	k_reset,
	k_panic,
	k_oversampling_mode,

	// end of parameter tags, k_num_params sets the number of available controls for the host UI
	k_num_params,

	// invisible parameters
	k_samplerate,
	k_tempo
))
#endif

#if defined(OPS_SEULA)
CONSTRUCT_ENUM(eStereoFilter, SINGLE_ARG(
	// Delay/echo params
	k_filter_mode = 0,
	k_filter_cutoff,
	k_filter_resonance,

	k_num_user_params, // number of "real" fx params (all params minus gui junk)

										 // additional gui params
	k_icaptioncontrol,
	k_fileopen,
	k_filesave,
	k_reset,
	k_panic,

	// end of parameter tags, k_num_params sets the number of available controls for the host UI
	k_num_params,

	// invisible parameters
	k_samplerate,
	k_tempo
))
#endif

#if defined(OPS_HARMONIKKA)
CONSTRUCT_ENUM(eSSynth, SINGLE_ARG(
	k_osc1_waveform = 0,
	k_osc1_semi,
	k_osc1_detune,
	k_osc1_volume,
	k_osc1_pw, // pulsewidth
	k_osc1_pwm, // pulsewidth modulation

	k_osc2_waveform,
	k_osc2_semi,
	k_osc2_detune,
	k_osc2_volume,
	k_osc2_pw,
	k_osc2_pwm,

	k_osc3_waveform,
	k_osc3_semi,
	k_osc3_detune,
	k_osc3_volume,
	k_osc3_pw,
	k_osc3_pwm,

	k_lfo1_waveform,
	k_lfo1_speed,
	k_lfo1_startphase,
	k_lfo1_keytrack,
	k_lfo1_mod_dest,
	k_lfo1_mod_amount,

	k_lfo2_waveform,
	k_lfo2_speed,
	k_lfo2_startphase,
	k_lfo2_keytrack,
	k_lfo2_mod_dest,
	k_lfo2_mod_amount,

	k_lfo3_waveform,
	k_lfo3_speed,
	k_lfo3_startphase,
	k_lfo3_keytrack,
	k_lfo3_mod_dest,
	k_lfo3_mod_amount,

	k_pitch_env_mod,
	k_pitch_attack,
	k_pitch_decay,
	k_pitch_sustain,
	k_pitch_release,

	k_filter1_mode,
	k_filter1_cutoff,
	k_filter1_resonance,
	k_filter1_drive,

	k_filter1_keytrack,
	k_filter1_vel_mod, // modulate by velocity
	k_filter1_env_mod, // modulate by envelope

	k_filter2_mode,
	k_filter2_cutoff,
	k_filter2_resonance,
	k_filter2_drive,

	k_filter2_keytrack,
	k_filter2_vel_mod,
	k_filter2_env_mod,

	k_filter_attack,
	k_filter_decay,
	k_filter_sustain,
	k_filter_release,

	k_amp_env_vel_mod,

	k_env_length, // envelope length (up to 10 seconds)
	k_filter_routing, // filter routing: serial or parallel
	k_poly_mode, // polyphony mode
	k_glide_type, // glide,portamento etc.
	k_glide_time,
	k_env_mode, // envelope mode / shape (linear, digital, analog emulation)

	k_amp_attack,
	k_amp_decay,
	k_amp_sustain,
	k_amp_release,

	k_tempo,
	k_pan,

	k_filter1_output_level,
	k_filter2_output_level,

	k_synth_type,
	k_fm_algorithm,
	k_osc3_feedback,

	// In FM mode use as ratio, in sample playback mode use as sample slot (0-20)
	k_osc1_ratio,
	k_osc2_ratio,
	k_osc3_ratio,

	k_num_user_params, // number of "real" synth params (all params minus gui junk)

	// additional gui params
	k_icaptioncontrol,
	k_fileopen,
	k_filesave,
	k_reset,
	k_panic,

	// end of parameter tags, k_num_params sets the number of available controls for the host UI
	k_num_params,

	// invisible parameters
	k_samplerate,
	k_pitchbend
))

CONSTRUCT_ENUM(eEnvModes, SINGLE_ARG(
	linear, digital, analog,
	num_items
))

enum envelopes
{
	k_pitch_env, k_filter_env, k_amp_env
};

CONSTRUCT_ENUM(eFilterRoutings, SINGLE_ARG(
	serial, parallel,
	num_items
))

CONSTRUCT_ENUM(eModDests, SINGLE_ARG(
	none, pitch, pitch_osc1, pitch_osc2, pitch_osc3, amp, amp_osc1, amp_osc2, amp_osc3, filter1_cutoff, filter2_cutoff, pwm,
	num_items
))

CONSTRUCT_ENUM(eGlideTypes, SINGLE_ARG(
	off, portamnt, glissando,
	num_items
))

CONSTRUCT_ENUM(eSynthTypes, SINGLE_ARG(
	substractive, fm_algo1, fm_algo2, fm_algo3, fm_algo4, fm_algo5, fm_algo6,
	num_items
))

CONSTRUCT_ENUM(ePolyModes, SINGLE_ARG(
	poly, mono, legato,
	num_items
))
#endif

#if defined (OPS_KUORO) || defined (OPS_HARMONIKKA)
CONSTRUCT_ENUM(eOscWaveforms, SINGLE_ARG(
	sine, blepsaw, blepsquare, bleppulse, noise, sampleandhold, rawsaw, rawsquare, rawpulse, rawtriangle, off,
	sampleplayback,
	num_items
))

enum oscillator_modes
{
	k_oscmode_oscillator, k_oscmode_lfo,
	k_num_oscmodes
};
#endif

#if defined(OPS_PAPUKAIJA)
CONSTRUCT_ENUM(eDelayModes, SINGLE_ARG(
	off, mono, stereo, pingpong, cross,
	num_items
))
#endif

#if defined(OPS_MURTO)
CONSTRUCT_ENUM(eDistortionModes, SINGLE_ARG(
	off, tanh, clip, wrap, foldback, bit_depth, sample_rate, pass_through,
	num_items
))

CONSTRUCT_ENUM(eOversamplingModes, SINGLE_ARG(
	off, x2, x4, x8, fixed_x2, fixed_x4, fixed_x8,
	num_items
))
#endif

#if defined(OPS_HARMONIKKA) || defined(OPS_TYNNYRI) || defined(OPS_SEULA) || defined(OPS_KUORO)
CONSTRUCT_ENUM(eFilterModes, SINGLE_ARG(
	// lowpass, highpass, bandpass/skirt (constant skirt gain), bandpass/peak (constant peak gain), bandstop, only drive, bypass,
	off, lowpass, highpass, bandpass_skirt, bandpass_peak, bandstop, only_drive,
	num_items
	// TODO: decide whether to include the following:
	//k_filter_bell, k_filter_lowshelf, k_filter_highshelf, k_filter_allpass
))

#endif

enum event_nums
{
// numeric event codes, not all of them used at the moment
	k_event_num_standard_note_on = 0,
	k_event_num_standard_note_off = 1,
	k_event_num_standard_pitch_bend = 2,
	k_event_num_standard_control_code = 3,
	k_event_num_custom_nrpn = 4,
	k_event_num_global_division = 5,
	k_event_num_global_tempo = 6
};

#endif
