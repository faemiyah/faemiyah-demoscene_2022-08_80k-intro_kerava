#pragma once

#ifndef COMMON_HPP
#define COMMON_HPP

#define GHETTO_AUDIO
#if defined(GHETTO_AUDIO)
#define APPROXIMATE_TANH
#define APPROXIMATE_SIN
#define APPROXIMATE_TAN
#endif
//#define ALTERNATIVE_FMODF

#if !defined(WIN32)
#include "../dnload.h"
#elif defined(TEST_EXECUTION)
#include "dnload.h"
#include <memory>
#endif

#if defined(DNLOAD_USE_LD) && defined(DEBUG)
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <boost/exception/diagnostic_information.hpp>
#endif

#if defined(OPS_GUI)
#include "resource.h"
#include <string.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#endif

/** \file
 * Common header file with some generally useful stuff.
 *
 * May be used with or without dnload.h
 */

#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
 /** \cond */
#define _USE_MATH_DEFINES
/** \endcond */
#endif

#if defined(WIN32)
#include <math.h>
#include <stdint.h>
#include <cstdio>
#include <sstream>
#endif

#if defined(APPROXIMATE_TANH)
#define ops_tanhf common::rational_tanh
#elif defined(WIN32)
#define ops_tanhf tanhf
#else
#define ops_tanhf dnload_tanhf
#endif

#if defined(APPROXIMATE_TAN)
#define ops_tanf common::fast_tan
#elif defined(WIN32)
#define ops_tanf tanf
#else
#define ops_tanf dnload_tanf
#endif

#if defined(APPROXIMATE_SIN)
#define ops_sinf common::fast_sin
#elif defined(WIN32)
#define ops_sinf sinf
#else
#define ops_sinf dnload_sinf
#endif

#if defined(WIN32)
#define ops_roundf common::croundf
#elif defined(DNLOAD_USE_LD)
#define ops_roundf roundf
#endif
#if defined(WIN32) || defined(DNLOAD_USE_LD)
#define ops_exp2f exp2f
#define ops_powf powf
#define ops_sqrtf sqrtf
#define ops_expf expf
#define ops_logf logf
#define ops_log2f log2f
#define ops_memset memset
#else
#define ops_exp2f dnload_exp2f
#define ops_powf dnload_powf
#define ops_roundf dnload_roundf
#define ops_sqrtf dnload_sqrtf
#define ops_expf dnload_expf
#define ops_logf dnload_logf
#define ops_log2f dnload_log2f
#define ops_memset dnload_memset
#endif

#if defined(ALTERNATIVE_FMODF)
#define ops_fmodf common::mfmodf
#elif defined(WIN32) || defined(DNLOAD_USE_LD)
#define ops_fmodf fmodf
#else
#define ops_fmodf dnload_fmodf
#endif


/** Floating-point samplerate. */
#define SAMPLERATE 44100.0f

#if defined(OPS_GUI)
#define M_SAMPLERATE m_samplerate
#else
#define M_SAMPLERATE SAMPLERATE
#endif

/** Interval for internal state calculations in cycles*/
#define STATE_CALC_INTERVAL 0

/** Single-precision M_SQRT1_2. */
#define ONE_OVER_SQRT2 static_cast<float>(M_SQRT1_2)

/** Single-precision M_PI. */
#if defined(WIN32)
#define PII static_cast<float>(M_PI)
#else
#define PII 3.1415927f
#endif

/** Single-precision two times pi. */
#if defined(WIN32)
#define TAU static_cast<float>(M_PI * 2.0)
#else
#define TAU 6.2831854f
#endif
namespace common
{
	//----------------------------------------------------------------------------
#if defined(APPROXIMATE_TANH)
	// cschueler's rational tanh approximation from http://www.musicdsp.org/showone.php?id=238
	inline float rational_tanh(float x)
	{
		if (x < -3.0f)
		{
			return -1.0f;
		}
		else if (x > 3.0f)
		{
			return 1.0f;
		}
		else
		{
			return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
		}
	}
#endif

#if defined(APPROXIMATE_SIN) || defined(APPROXIMATE_TAN)
#define HALF_PI		 (0.5f * PII)

	//----------------------------------------------------------------------------
	// based on Michael Baczynski's sine approximation at http://lab.polygonal.de/?p=205
	inline float fast_sin(float x)
	{
		// always wrap input angle to -PI..PI
		while (fabs(x) > PII)
		{
			if (x < -PII)
			{
				x += TAU;
			}
			else
			{
				if (x > PII)
				{
					x -= TAU;
				}
			}
		}

		// compute sine
		if (x < 0.0f)
		{
			return (1.27323954f * x) + (0.405284735f * x * x);
		}
		else
		{
			return (1.27323954f * x) - (0.405284735f * x * x);
		}
	}

	//----------------------------------------------------------------------------
	inline float fast_cos(float x)
	{
		return fast_sin(x + HALF_PI);
	}

	//----------------------------------------------------------------------------
	inline float fast_tan(float x)
	{
		return fast_sin(x) / fast_cos(x);
	}
#endif

	//----------------------------------------------------------------------------
#if defined(ALTERNATIVE_FMODF)
	inline float mfmodf(float x, float y) { float a; return ((a = x / y) - (int)a) * y; }
#endif

#if defined(WIN32)
	/** \brief Max function
	 *
	 * @param lhs Left-hand-side operand.
	 * @param rhs Right-hand-side operand.
	 * @return Maximum value.
	 */
	static inline float cfmaxf(float lhs, float rhs)
	{
		return (lhs >= rhs) ? lhs : rhs;
	}
#endif

#if defined(WIN32)
	/** \brief Min function
	 *
	 * @param lhs Left-hand-side operand.
	 * @param rhs Right-hand-side operand.
	 * @return Minimum value.
	 */
	static inline float cfminf(float lhs, float rhs)
	{
		return (lhs <= rhs) ? lhs : rhs;
	}
#endif

	/** \brief Float round to int.
	 *
	 * @param op Floating point input.
	 * @return Integer output.
	 */
	static inline int clrintf(float op)
	{
#if defined(WIN32)
		return static_cast<int>(op + 0.5f);
#elif defined(DNLOAD_USE_LD)
		return static_cast<int>(lrintf(op));
#else
		return static_cast<int>(dnload_lrintf(op));
#endif
	}

	/** \brief Float round.
	 *
	 * @param op Floating point input.
	 * @return Rounded result.
	 */
	static inline float croundf(float op)
	{
#if defined(WIN32)
		return floorf(op + 0.5f);
#elif defined(DNLOAD_USE_LD)
		return roundf(op);
#else
		return static_cast<float>(dnload_lrintf(op));
#endif
	}

	/** \brief Clamp function.
	*
	* As in GLSL.
	*
	* @param op Input operand.
	* @param min Minimum value.
	* @param max Maximum value.
	* @retuen Clamped input value.
	*/
	static float cclampf(float op, float min, float max)
	{
            return std::max(std::min(op, max), min);
	}

	//----------------------------------------------------------------------------
	/** \brief Clamp value to unit size.
	*
	* @param op Input operand.
	* @retuen Input clamped to unit size.
	*/
	static float cclamp1f(float op)
	{
            return cclampf(op, -1.0f, 1.0f);
	}

	//----------------------------------------------------------------------------
	// Denormalization killing with dc as presented by Laurent de Soras in
	// http://ldesoras.free.fr/doc/articles/denormal-en.pdf
	static void add_dc(float& val)
	{
		static const float antidenormal_dc = 0.000000000000000001f;
		val += antidenormal_dc;
	}
}

#endif
