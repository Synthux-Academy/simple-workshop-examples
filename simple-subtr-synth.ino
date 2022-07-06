// Title: moogladder
// Description: Sweeps the cutoff with an lfo
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

float filter_freq;
float filter_res;
float atten_fm;

static MoogLadder flt;
static Oscillator osc, lfo;

void MyCallback(float **in, float **out, size_t size) {
  float osc_sample, output, lfo_sample;
    
  for (size_t i = 0; i < size; i++) {
    lfo_sample = lfo.Process();
    
    osc.SetFreq(100.0 + 100.0 * lfo_sample);
    osc_sample = osc.Process();

    output = flt.Process(osc_sample);

    out[0][i] = output;
    out[1][i] = output;
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

  DAISY.begin(MyCallback);
}

void loop() {
  filter_freq = (1023 - analogRead(A0)) / 1023.0 * 5000.0;
  flt.SetFreq(filter_freq);
  filter_res = (1023 - analogRead(A1)) / 1023.0 - 0.2;
  flt.SetRes(filter_res < 0.0 ? 0.0 : filter_res);
  lfo.SetFreq(.4);
}
