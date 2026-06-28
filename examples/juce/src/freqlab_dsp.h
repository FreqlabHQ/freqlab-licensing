#pragma once

// freqlab_dsp.h - opt-in DSP-fold (anti-tamper).
//
// OPT-IN: nothing happens unless you (1) keep this file in your project and
// (2) place a `// @freqlab:dsp-fold` tag at the point in your DSP where you
// want to fold in the coefficient. The freqlab cloud build's transformer then
// injects a call to `freqlab_dsp_fold(buffer)` at that tag.
//
// What it does: freqlab_dsp_coefficient() is the identity (a no-op, gain 1.0)
// in local builds, licensed cloud builds, and shadow-mode builds. In an armed
// cloud build a bypassed/invalid license makes it drift off 1.0, so the audio
// is audibly wrong rather than cleanly silent - the one thing a simple
// "NOP the license check" crack can't defeat.
//
// You own where it folds in. The default below applies it as an output gain.
// To fold it elsewhere (an envelope, a filter coefficient, a wet/dry mix),
// edit freqlab_dsp_fold, or call freqlab_dsp_gain() directly in your DSP and
// drop the tag.
//
// Both functions are lock-free and safe to call from the audio thread.

#include "freqlab_licensing.h"
#include <juce_audio_basics/juce_audio_basics.h>

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
inline void freqlab_dsp_fold(juce::AudioBuffer<float>& buffer)
{
    buffer.applyGain(freqlab_dsp_gain());
}
