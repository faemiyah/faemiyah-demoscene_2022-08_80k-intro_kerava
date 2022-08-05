#pragma once

#ifndef ONEPOLE_FILTER_HPP
#define ONEPOLE_FILTER_HPP

/* \brief One Pole Low Pass Filter class.
*
* Primarily intended to be used in Dattorro's plate reverb,
* hence the inclusion of a gain reversal option
* (bandwidth vs damping)
*
*/

class OnePoleLPF
{
public:
  //----------------------------------------------------------------------------
  OnePoleLPF(void)
  {
    m_gain = 0.5f;
    m_sample = 0.0f;
  }

  //----------------------------------------------------------------------------
  ~OnePoleLPF(void)
  {
  }

  void setGain(float gain)
  {
    m_gain = common::cclampf(gain, 0.0f, 1.0f);
  }

  //----------------------------------------------------------------------------
  // If reverse_gain is true, 0.0f is max bandwidth, 1.0f is fully closed (used for damping)
  float process(float in, bool reverse_gain = false)
  {
    if (reverse_gain)
      m_sample = ((1.0f - m_gain) * in) + (m_gain * m_sample);
    else
      m_sample = (m_gain * in) + ((1.0f - m_gain) * m_sample);

    return m_sample;
  }

private:
  float m_sample;
  float m_gain;
};

#endif
