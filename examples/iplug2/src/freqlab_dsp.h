#pragma once

// freqlab_dsp.h - opt-in DSP-fold (anti-tamper). See the JUCE example's
// header comment for the full rationale; the short version:
//
// OPT-IN: place a `// @freqlab:dsp-fold` tag in your DSP and the freqlab cloud
// build injects a call to `freqlab_dsp_fold(outputs, nChans, nFrames)` there.
// The coefficient is the identity (gain 1.0, a no-op) locally / when licensed
// / in shadow mode; in an armed cloud build a bypassed license drifts it off
// 1.0 so cracked audio is audibly wrong, not cleanly silent.
//
// You own where it folds in - edit freqlab_dsp_fold, or call freqlab_dsp_gain()
// directly in your DSP. Lock-free, audio-thread safe.

#include "freqlab_licensing.h"
#include "IPlug_include_in_plug_hdr.h"

// The fold coefficient as a gain multiplier: ~1.0 when licensed, off-1.0 when
// not (in an armed build). Fold this into any DSP value you like.
inline float freqlab_dsp_gain()
{
    return static_cast<float>(
        static_cast<double>(::freqlab::licensing::freqlab_dsp_coefficient()) /
        static_cast<double>(::freqlab::licensing::FREQLAB_DSP_IDENTITY));
}

// Default fold: scale the output by the coefficient. The transformer injects a
// call to this at your `// @freqlab:dsp-fold` tag. Edit freely.
inline void freqlab_dsp_fold(iplug::sample** outputs, int nChans, int nFrames)
{
    const iplug::sample g = static_cast<iplug::sample>(freqlab_dsp_gain());
    for (int c = 0; c < nChans; ++c)
        for (int s = 0; s < nFrames; ++s)
            outputs[c][s] *= g;
}
