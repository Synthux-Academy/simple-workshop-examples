////////////////////////////////////////////////////////////////
///////////////////// VARIABLES & LIBRARIES ////////////////////

#include "DaisyDuino.h"

static Oscillator osc;

////////////////////////////////////////////////////////////////
///////////////////// START SYNTH SETUP ////////////////////////


void setup() {
  float sample_rate;
  // DAISY SETUP
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();

  // OSCILLATOR SETUP
  osc.Init(sample_rate);
  osc.SetFreq(440);
  osc.SetAmp(0.5);
  osc.SetWaveform(osc.WAVE_TRI);

  // DAISY SETUP
  DAISY.begin(ProcessAudio);
}


///////////////////// END SYNTH SETUP //////////////////////////
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
///////////////////// PROCESSING AUDIO SAMPLES (LOOP) //////////


void ProcessAudio(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float sample = osc.Process();
    out[0][i] = sample;
    out[1][i] = sample;
  }
}


////////////////////////////////////////////////////////////////
///////////////////// START CONTROLS LOOP //////////////////////


void loop() {}
