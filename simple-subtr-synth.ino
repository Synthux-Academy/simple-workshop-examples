// Title: moogladder
// Description: Sweeps the cutoff with an lfo
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

float filter_freq;
float filter_res;
float atten_fm;
float verb_fback;
float wet_amt;

static MoogLadder flt;
static Oscillator osc, lfo;
static ReverbSc verb;

void AudioCallback(float **in, float **out, size_t size) {
  float osc_sample, output, lfo_sample;

  for (size_t i = 0; i < size; i++) {
    float rev_tail0, rev_tail1;

    lfo_sample = lfo.Process();

    // lfo_sample is between -1 and 1
    osc.SetFreq(200.0 + 100.0 * lfo_sample);
    osc_sample = osc.Process();

    output = flt.Process(osc_sample);

    verb.Process(output, output, &rev_tail0, &rev_tail1);

    out[0][i] = (1 - wet_amt) * output + wet_amt * rev_tail0;
    out[1][i] = (1 - wet_amt) * output + wet_amt * rev_tail1;
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();

  // initialize Moogladder object
  flt.Init(sample_rate);
  flt.SetRes(0.7);

  // set parameters for sine oscillator object
  lfo.Init(sample_rate);
  lfo.SetWaveform(Oscillator::WAVE_TRI);
  lfo.SetAmp(1);
  lfo.SetFreq(.4);

  // set parameters for sine oscillator object
  osc.Init(sample_rate);
  osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
  osc.SetFreq(100);
  osc.SetAmp(0.25);

  // set reverb params
  verb.Init(sample_rate);
  verb.SetFeedback(0.2);
  verb.SetLpFreq(16000);

  DAISY.begin(AudioCallback);
}

// A0 filter freq
// A1 resonance
// A2 lfo freq
// A3 osc freq
// A4 lfo amt -> filter
// A5 room size
// A6 wet + dry
// A7 master volume

void loop() {
  // TODO: use Arduino's map() function
  // 0 - 5000.0
  filter_freq = (1023 - analogRead(A0)) / 1023.0 * 5000.0;
  flt.SetFreq(filter_freq);
  // 0 - 0.8
  filter_res = (1023 - analogRead(A1)) / 1023.0 / 1.25;
  flt.SetRes(filter_res < 0.0 ? 0.0 : filter_res);
  lfo.SetFreq(.4);
  // 0.4 - 1.0
  verb_fback = 0.4 + (1023 - analogRead(A5)) / 1023.0 * 0.6;
  verb.SetFeedback(verb_fback);
  wet_amt = (1023 - analogRead(A6)) / 1023.0;
}
