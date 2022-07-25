// Title: simple subtractive synth
// Description: oscillators! filters! lfos! reverb!
// Hardware: Daisy Seed
// Author: Chris Maniewski

#include "DaisyDuino.h"

DaisyHardware hw;

float filter_freq;
float filter_res;
float verb_fback;
float wet_amt;
float modulator_freq;
float fm_amt;
float carrier_freq;
float master_vol;

static MoogLadder flt;
static Oscillator carrier, modulator;
static ReverbSc verb;
static Chorus chorus;

void AudioCallback(float **in, float **out, size_t size) {
  float carrier_sample, modulator_sample, output;

  for (size_t i = 0; i < size; i++) {
    float rev_tail0, rev_tail1, flt_freq, chorus0, chorus1;

    modulator_sample = (modulator.Process() + 0.5);

    flt.SetFreq(filter_freq);

    carrier.SetFreq(carrier_freq + fm_amt * 100 * modulator_sample * carrier_freq);
    carrier_sample = carrier.Process();

    output = flt.Process(carrier_sample);

    chorus.Process(output);
    chorus0 = chorus.GetRight();
    chorus1 = chorus.GetRight();

    verb.Process(chorus0, chorus1, &rev_tail0, &rev_tail1);
    out[0][i] = ((1 - wet_amt) * output + wet_amt * rev_tail0) * master_vol;

    out[1][i] = ((1 - wet_amt) * output + wet_amt * rev_tail1) * master_vol;

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
  carrier.Init(sample_rate);
  carrier.SetWaveform(Oscillator::WAVE_SIN);
  carrier.SetFreq(220);
  carrier.SetAmp(0.25);

  // set parameters for sine oscillator object
  modulator.Init(sample_rate);
  modulator.SetWaveform(Oscillator::WAVE_SIN);
  // ac-coupled, between -.5 and .5
  modulator.SetAmp(0.5);
  modulator.SetFreq(220);

  // set chorus params
  chorus.Init(sample_rate);
  chorus.SetLfoFreq(.33f, .2f);
  chorus.SetLfoDepth(1.f, 1.f);
  chorus.SetDelay(.75f, .9f);

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
  modulator_freq = (1023.0 - analogRead(A2)) / 1023 * 5 * carrier_freq;
  modulator.SetFreq(modulator_freq);
  // 0 - 5000.0
  filter_freq = (1023 - analogRead(A0)) / 1023.0 * 5000.0;
  // 0 - 0.8
  filter_res = (1023 - analogRead(A1)) / 1023.0 / 1.25;
  flt.SetRes(filter_res < 0.0 ? 0.0 : filter_res);
  // 0.4 - 1.0
  verb_fback = 0.4 + (1023 - analogRead(A5)) / 1023.0 * 0.6;
  verb.SetFeedback(verb_fback);
  wet_amt = (1023 - analogRead(A6)) / 1023.0;
  // one octave
  carrier_freq =
      semitone_to_hertz(map((1023 - analogRead(A3)), 0, 1023, 0, 12));
  // 0 - 1.0
  fm_amt = (1023 - analogRead(A4)) / 1023.0;
  // 0 - 1.0
  master_vol = (1023 - analogRead(A7)) / 1023.0;
}

float semitone_to_hertz(uint8_t note_number) {
  return 220 * pow(2, ((float)note_number - 0) / 12);
}
