// freqlab_dsp.rs - opt-in DSP-fold (anti-tamper).
//
// OPT-IN: nothing happens unless you (1) keep this file and add
// `mod freqlab_dsp;` to your crate, and (2) place a `// @freqlab:dsp-fold` tag
// at the point in your DSP where you want to fold in the coefficient. The
// freqlab cloud build's transformer then injects a call to
// `freqlab_dsp_fold(buffer)` at that tag.
//
// freqlab_dsp_coefficient() is the identity (a no-op, gain 1.0) in local
// builds, licensed cloud builds, and shadow-mode builds. In an armed cloud
// build a bypassed/invalid license makes it drift off 1.0 so the audio is
// audibly wrong rather than cleanly silent - the one thing a simple "NOP the
// license check" crack can't defeat.
//
// You own where it folds in. The default below applies it as an output gain;
// to fold elsewhere edit `freqlab_dsp_fold`, or call `freqlab_dsp_gain()`
// directly in your DSP and drop the tag. Lock-free, audio-thread safe.

use freqlab_licensing as licensing;
use nih_plug::prelude::Buffer;

// The fold coefficient as a gain multiplier: ~1.0 when licensed, off-1.0 when
// not (in an armed build). Fold this into any DSP value you like.
pub fn freqlab_dsp_gain() -> f32 {
    (licensing::freqlab_dsp_coefficient() as f64 / licensing::FREQLAB_DSP_IDENTITY as f64) as f32
}

// Default fold: scale the output by the coefficient. The transformer injects a
// call to this at your `// @freqlab:dsp-fold` tag. Edit freely.
pub fn freqlab_dsp_fold(buffer: &mut Buffer) {
    let g = freqlab_dsp_gain();
    for channel_samples in buffer.iter_samples() {
        for sample in channel_samples {
            *sample *= g;
        }
    }
}
