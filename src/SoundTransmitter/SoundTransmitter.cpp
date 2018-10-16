// ----------------------------------------------------------------------------
// SoundTransmitter.cpp
//
//
// Authors:
// Peter Polidoro peterpolidoro@gmail.com
// ----------------------------------------------------------------------------
#include "../SoundTransmitter.h"


namespace sound_transmitter
{
// GUItool: begin automatically generated code
AudioSynthNoiseWhite     g_noise_left;   //xy=85,251
AudioSynthNoiseWhite     g_noise_right;  //xy=89,288
AudioSynthWaveformSine   g_tone_left;    //xy=250,176
AudioSynthWaveformSine   g_tone_right;   //xy=256,215
AudioPlaySdWav           g_play_sd_wav;  //xy=263,102
AudioFilterBiquad        g_biquad_left;  //xy=263,251
AudioPlaySdRaw           g_play_sd_raw;  //xy=264,139
AudioFilterBiquad        g_biquad_right; //xy=265,289
AudioMixer4              g_mixer_left;   //xy=500,111
AudioMixer4              g_mixer_right;  //xy=506,187
AudioMixer4              g_mixer_dac;    //xy=702,188
AudioOutputI2S           g_stereo_speaker; //xy=717,126
AudioOutputAnalog        g_pcb_speaker;  //xy=890,188
AudioConnection          patchCord1(g_noise_left, g_biquad_left);
AudioConnection          patchCord2(g_noise_right, g_biquad_right);
AudioConnection          patchCord3(g_tone_left, 0, g_mixer_left, 2);
AudioConnection          patchCord4(g_tone_right, 0, g_mixer_right, 2);
AudioConnection          patchCord5(g_play_sd_wav, 0, g_mixer_left, 0);
AudioConnection          patchCord6(g_play_sd_wav, 1, g_mixer_right, 0);
AudioConnection          patchCord7(g_biquad_left, 0, g_mixer_left, 3);
AudioConnection          patchCord8(g_play_sd_raw, 0, g_mixer_left, 1);
AudioConnection          patchCord9(g_play_sd_raw, 0, g_mixer_right, 1);
AudioConnection          patchCord10(g_biquad_right, 0, g_mixer_right, 3);
AudioConnection          patchCord11(g_mixer_left, 0, g_stereo_speaker, 0);
AudioConnection          patchCord12(g_mixer_left, 0, g_mixer_dac, 0);
AudioConnection          patchCord13(g_mixer_right, 0, g_stereo_speaker, 1);
AudioConnection          patchCord14(g_mixer_right, 0, g_mixer_dac, 1);
AudioConnection          patchCord15(g_mixer_dac, g_pcb_speaker);
AudioControlSGTL5000     g_sgtl5000;     //xy=498,36
// GUItool: end automatically generated code
}

using namespace sound_transmitter;

SoundTransmitter::SoundTransmitter()
{
  codec_enabled_ = false;
  playing_ = false;
  pulsing_ = false;
}

void SoundTransmitter::setup()
{
  // Parent Setup
  ModularDeviceBase::setup();

  // Reset Watchdog
  resetWatchdog();

  // Audio Setup
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(constants::AUDIO_MEMORY_BLOCK_COUNT);
  AudioMemoryUsageMaxReset();

  enableAudioCodec();

  g_pcb_speaker.analogReference(constants::pcb_speaker_reference);

  // Event Controller Setup
  event_controller_.setup();

  // Pin Setup
  pinMode(constants::pcb_speaker_enable_pin,OUTPUT);
  digitalWrite(constants::pcb_speaker_enable_pin,HIGH);

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware
  modular_server_.addHardware(constants::hardware_info,
    pins_);

  // Pins
  modular_server::Pin & bnc_a_pin = modular_server_.pin(modular_device_base::constants::bnc_a_pin_name);

  modular_server::Pin & btn_a_pin = modular_server_.pin(modular_device_base::constants::btn_a_pin_name);


  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
    properties_,
    parameters_,
    functions_,
    callbacks_);

  // Properties
  modular_server::Property & trigger_tone_frequency_property = modular_server_.createProperty(constants::trigger_tone_frequency_property_name,constants::frequency_default);
  trigger_tone_frequency_property.setRange(constants::frequency_min,constants::frequency_max);
  trigger_tone_frequency_property.setUnits(constants::hz_units);

  modular_server::Property & trigger_tone_volume_property = modular_server_.createProperty(constants::trigger_tone_volume_property_name,constants::volume_default);
  trigger_tone_volume_property.setRange(constants::volume_min,constants::volume_max);
  trigger_tone_volume_property.setUnits(constants::percent_units);

  modular_server::Property & trigger_tone_duration_property = modular_server_.createProperty(constants::trigger_tone_duration_property_name,constants::duration_default);
  trigger_tone_duration_property.setRange(constants::duration_min,constants::duration_max);
  trigger_tone_duration_property.setUnits(constants::ms_units);

  // Parameters
  modular_server::Parameter & frequency_parameter = modular_server_.createParameter(constants::frequency_parameter_name);
  frequency_parameter.setRange(constants::frequency_min,constants::frequency_max);
  frequency_parameter.setUnits(constants::hz_units);

  modular_server::Parameter & volume_parameter = modular_server_.createParameter(constants::volume_parameter_name);
  volume_parameter.setRange(constants::volume_min,constants::volume_max);
  volume_parameter.setUnits(constants::percent_units);

  // Functions
  modular_server::Function & play_tone_function = modular_server_.createFunction(constants::play_tone_function_name);
  play_tone_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playToneHandler));
  play_tone_function.addParameter(frequency_parameter);
  play_tone_function.addParameter(volume_parameter);

  modular_server::Function & play_noise_function = modular_server_.createFunction(constants::play_noise_function_name);
  play_noise_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playNoiseHandler));
  play_noise_function.addParameter(volume_parameter);

  modular_server::Function & stop_function = modular_server_.createFunction(constants::stop_function_name);
  stop_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::stopHandler));

  modular_server::Function & is_playing_function = modular_server_.createFunction(constants::is_playing_function_name);
  is_playing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::isPlayingHandler));
  is_playing_function.setResultTypeBool();

  // Callbacks
  modular_server::Callback & trigger_tone_callback = modular_server_.createCallback(constants::trigger_tone_callback_name);
  trigger_tone_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&SoundTransmitter::triggerToneHandler));
  trigger_tone_callback.addProperty(trigger_tone_frequency_property);
  trigger_tone_callback.addProperty(trigger_tone_volume_property);
  trigger_tone_callback.addProperty(trigger_tone_duration_property);
  trigger_tone_callback.attachTo(bnc_a_pin,modular_server::constants::pin_mode_interrupt_falling);
  trigger_tone_callback.attachTo(btn_a_pin,modular_server::constants::pin_mode_interrupt_falling);


}

void SoundTransmitter::playTone(const size_t frequency,
  const long volume)
{
  stop();
  if ((volume <= 0) || (frequency < constants::frequency_stop_threshold))
  {
    return;
  }
  audio_type_playing_ = constants::TONE_TYPE;
  g_tone_left.amplitude(0);
  g_tone_left.frequency(frequency);
  g_tone_left.amplitude(1);
  g_tone_right.amplitude(0);
  g_tone_right.frequency(frequency);
  g_tone_right.amplitude(1);
  setVolume(volume);
  playing_ = true;
}

void SoundTransmitter::playNoise(const long volume)
{
  stop();
  if (volume <= 0)
  {
    return;
  }
  audio_type_playing_ = constants::NOISE_TYPE;
  g_noise_left.amplitude(1);
  g_noise_right.amplitude(1);
  setVolume(volume);
  playing_ = true;
}

void SoundTransmitter::stop()
{
  if (playing_)
  {
    switch (audio_type_playing_)
    {
      case constants::TONE_TYPE:
      {
        g_tone_left.amplitude(0);
        g_tone_right.amplitude(0);
      }
      case constants::NOISE_TYPE:
      {
        g_noise_left.amplitude(0);
        g_noise_right.amplitude(0);
      }
    }
    playing_ = false;
  }
}

bool SoundTransmitter::isPlaying()
{
  return playing_;
}

bool SoundTransmitter::codecEnabled()
{
  return codec_enabled_;
}

int SoundTransmitter::addTonePwm(const size_t frequency,
  const long volume,
  const long delay,
  const long period,
  const long on_duration,
  const long count)
{
  if (indexed_pulses_.full())
  {
    return constants::bad_index;
  }
  sound_transmitter::constants::PulseInfo pulse_info;
  pulse_info.frequency = frequency;
  pulse_info.volume = volume;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::playToneHandler),
    makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopHandler),
    delay,
    period,
    on_duration,
    count,
    index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

void SoundTransmitter::enableAudioCodec()
{
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);
  if (digitalRead(SDA) && digitalRead(SCL))
  {
    // This may wait forever if the SDA & SCL pins lack
    // pullup resistors so check first
    codec_enabled_ = true;
    g_sgtl5000.enable();
  }
}

void SoundTransmitter::updatePlaying()
{
  if (playing_)
  {
    switch (audio_type_playing_)
    {
      case constants::TONE_TYPE:
      {
        break;
      }
      case constants::NOISE_TYPE:
      {
        break;
      }
    }
  }
}

void SoundTransmitter::setVolume(const long volume)
{
  double stereo_speaker_gain = 1.0;

  if (codecEnabled())
  {
    g_sgtl5000.volume(((double)volume*stereo_speaker_gain)/100.0);
  }

  double pcb_speaker_gain = 1.0;

  double pcb_speaker_total_gain = (constants::pcb_speaker_channel_gain*constants::pcb_speaker_pre_gain*(double)volume*pcb_speaker_gain)/100.0;
  g_mixer_dac.gain(0,pcb_speaker_total_gain);
  g_mixer_dac.gain(1,pcb_speaker_total_gain);
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray *
// ArduinoJson::JsonObject *
// const ConstantString *
//
// For more info read about ArduinoJson parsing https://github.com/janelia-arduino/ArduinoJson
//
// modular_server_.property(property_name).getValue(value) value type must match the property default type
// modular_server_.property(property_name).setValue(value) value type must match the property default type
// modular_server_.property(property_name).getElementValue(element_index,value) value type must match the property array element default type
// modular_server_.property(property_name).setElementValue(element_index,value) value type must match the property array element default type

void SoundTransmitter::startPwmHandler(int index)
{
  pulsing_ = true;
}

void SoundTransmitter::stopPwmHandler(int index)
{
  stop();
  indexed_pulses_.remove(index);
  pulsing_ = !indexed_pulses_.empty();
}

void SoundTransmitter::playToneHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  playTone(frequency,volume);
}

void SoundTransmitter::playNoiseHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  playNoise(volume);
}

void SoundTransmitter::stopHandler()
{
  stop();
}

void SoundTransmitter::isPlayingHandler()
{
  bool playing = isPlaying();
  modular_server_.response().returnResult(playing);
}

void SoundTransmitter::triggerToneHandler(modular_server::Pin * pin_ptr)
{
  long trigger_tone_frequency;
  modular_server_.property(constants::trigger_tone_frequency_property_name).getValue(trigger_tone_frequency);
  long trigger_tone_volume;
  modular_server_.property(constants::trigger_tone_volume_property_name).getValue(trigger_tone_volume);
  long trigger_tone_duration;
  modular_server_.property(constants::trigger_tone_duration_property_name).getValue(trigger_tone_duration);
  addTonePwm(trigger_tone_frequency,
    trigger_tone_volume,
    0,
    trigger_tone_duration*2,
    trigger_tone_duration,
    1);
}

void SoundTransmitter::playToneHandler(int index)
{
  size_t frequency = indexed_pulses_[index].frequency;
  long volume = indexed_pulses_[index].volume;
  playTone(frequency,volume);
}

void SoundTransmitter::playNoiseHandler(int index)
{
  long volume = indexed_pulses_[index].volume;
  playNoise(volume);
}

void SoundTransmitter::stopHandler(int index)
{
  stop();
}
