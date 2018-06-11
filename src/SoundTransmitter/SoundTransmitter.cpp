// ----------------------------------------------------------------------------
// SoundTransmitter.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
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
  path_played_[0] = 0;
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

  // Setup SD Card
  sd_interface_.setup();

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

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
                              properties_,
                              parameters_,
                              functions_,
                              callbacks_);

  // Properties
  modular_server::Property & volume_property = modular_server_.createProperty(constants::volume_property_name,constants::volume_default);
  volume_property.setRange(constants::volume_min,constants::volume_max);
  volume_property.setUnits(constants::percent_units);
  volume_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::setVolumeHandler));

  modular_server::Property & stereo_speaker_gain_property = modular_server_.createProperty(constants::stereo_speaker_gain_property_name,constants::stereo_speaker_gain_default);
  stereo_speaker_gain_property.setRange(constants::gain_min,constants::gain_max);
  stereo_speaker_gain_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::setVolumeHandler));

  modular_server::Property & pcb_speaker_gain_property = modular_server_.createProperty(constants::pcb_speaker_gain_property_name,constants::pcb_speaker_gain_default);
  pcb_speaker_gain_property.setRange(constants::gain_min,constants::gain_max);
  pcb_speaker_gain_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::setVolumeHandler));

  setVolumeHandler();

  // Parameters
  modular_server::Parameter & audio_path_parameter = modular_server_.createParameter(constants::audio_path_parameter_name);
  audio_path_parameter.setTypeString();

  modular_server::Parameter & frequency_parameter = modular_server_.createParameter(constants::frequency_parameter_name);
  frequency_parameter.setRange(constants::frequency_min,constants::frequency_max);
  frequency_parameter.setUnits(constants::hz_units);

  modular_server::Parameter & speaker_parameter = modular_server_.createParameter(constants::speaker_parameter_name);
  speaker_parameter.setTypeString();
  speaker_parameter.setSubset(constants::speaker_subset);

  modular_server::Parameter & delay_parameter = modular_server_.createParameter(constants::delay_parameter_name);
  delay_parameter.setRange(constants::delay_min,constants::delay_max);
  delay_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & period_parameter = modular_server_.createParameter(constants::period_parameter_name);
  period_parameter.setRange(constants::period_min,constants::period_max);
  period_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & on_duration_parameter = modular_server_.createParameter(constants::on_duration_parameter_name);
  on_duration_parameter.setRange(constants::on_duration_min,constants::on_duration_max);
  on_duration_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & count_parameter = modular_server_.createParameter(constants::count_parameter_name);
  count_parameter.setRange(constants::count_min,constants::count_max);

  modular_server::Parameter & pwm_index_parameter = modular_server_.createParameter(constants::pwm_index_parameter_name);
  pwm_index_parameter.setRange(0,constants::INDEXED_PULSES_COUNT_MAX-1);

  modular_server::Parameter & volume_parameter = modular_server_.createParameter(constants::volume_parameter_name);
  volume_parameter.setRange(constants::volume_min,constants::volume_max);
  volume_parameter.setUnits(constants::percent_units);

  modular_server::Parameter & bandwidth_parameter = modular_server_.createParameter(constants::bandwidth_parameter_name);
  bandwidth_parameter.setRange(constants::bandwidth_min,constants::bandwidth_max);
  bandwidth_parameter.setUnits(constants::octaves_units);

  // Functions
  modular_server::Function & get_audio_memory_usage_function = modular_server_.createFunction(constants::get_audio_memory_usage_function_name);
  get_audio_memory_usage_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getAudioMemoryUsageHandler));
  get_audio_memory_usage_function.setResultTypeLong();

  modular_server::Function & get_audio_memory_usage_max_function = modular_server_.createFunction(constants::get_audio_memory_usage_max_function_name);
  get_audio_memory_usage_max_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getAudioMemoryUsageMaxHandler));
  get_audio_memory_usage_max_function.setResultTypeLong();

  modular_server::Function & reset_audio_memory_usage_max_function = modular_server_.createFunction(constants::reset_audio_memory_usage_max_function_name);
  reset_audio_memory_usage_max_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::resetAudioMemoryUsageMaxHandler));

  modular_server::Function & get_audio_processor_usage_function = modular_server_.createFunction(constants::get_audio_processor_usage_function_name);
  get_audio_processor_usage_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getAudioProcessorUsageHandler));
  get_audio_processor_usage_function.setResultTypeDouble();

  modular_server::Function & get_audio_processor_usage_max_function = modular_server_.createFunction(constants::get_audio_processor_usage_max_function_name);
  get_audio_processor_usage_max_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getAudioProcessorUsageMaxHandler));
  get_audio_processor_usage_max_function.setResultTypeDouble();

  modular_server::Function & reset_audio_processor_usage_max_function = modular_server_.createFunction(constants::reset_audio_processor_usage_max_function_name);
  reset_audio_processor_usage_max_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::resetAudioProcessorUsageMaxHandler));

  modular_server::Function & get_sd_card_info_function = modular_server_.createFunction(constants::get_sd_card_info_function_name);
  get_sd_card_info_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getSDCardInfoHandler));
  get_sd_card_info_function.setResultTypeObject();

  modular_server::Function & get_audio_paths_function = modular_server_.createFunction(constants::get_audio_paths_function_name);
  get_audio_paths_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getAudioPathsHandler));
  get_audio_paths_function.setResultTypeArray();
  get_audio_paths_function.setResultTypeString();

  modular_server::Function & play_path_function = modular_server_.createFunction(constants::play_path_function_name);
  play_path_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playPathHandler));
  play_path_function.addParameter(audio_path_parameter);

  modular_server::Function & play_tone_function = modular_server_.createFunction(constants::play_tone_function_name);
  play_tone_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playToneHandler));
  play_tone_function.addParameter(frequency_parameter);
  play_tone_function.addParameter(speaker_parameter);

  modular_server::Function & play_tone_at_function = modular_server_.createFunction(constants::play_tone_at_function_name);
  play_tone_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playToneAtHandler));
  play_tone_at_function.addParameter(frequency_parameter);
  play_tone_at_function.addParameter(speaker_parameter);
  play_tone_at_function.addParameter(volume_parameter);

  modular_server::Function & play_noise_function = modular_server_.createFunction(constants::play_noise_function_name);
  play_noise_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playNoiseHandler));
  play_noise_function.addParameter(speaker_parameter);

  modular_server::Function & play_noise_at_function = modular_server_.createFunction(constants::play_noise_at_function_name);
  play_noise_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playNoiseAtHandler));
  play_noise_at_function.addParameter(speaker_parameter);
  play_noise_at_function.addParameter(volume_parameter);

  modular_server::Function & play_filtered_noise_function = modular_server_.createFunction(constants::play_filtered_noise_function_name);
  play_filtered_noise_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playFilteredNoiseHandler));
  play_filtered_noise_function.addParameter(frequency_parameter);
  play_filtered_noise_function.addParameter(bandwidth_parameter);
  play_filtered_noise_function.addParameter(speaker_parameter);

  modular_server::Function & play_filtered_noise_at_function = modular_server_.createFunction(constants::play_filtered_noise_at_function_name);
  play_filtered_noise_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::playFilteredNoiseAtHandler));
  play_filtered_noise_at_function.addParameter(frequency_parameter);
  play_filtered_noise_at_function.addParameter(bandwidth_parameter);
  play_filtered_noise_at_function.addParameter(speaker_parameter);
  play_filtered_noise_at_function.addParameter(volume_parameter);

  modular_server::Function & stop_function = modular_server_.createFunction(constants::stop_function_name);
  stop_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::stopHandler));

  modular_server::Function & is_playing_function = modular_server_.createFunction(constants::is_playing_function_name);
  is_playing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::isPlayingHandler));
  is_playing_function.setResultTypeBool();

  modular_server::Function & get_last_audio_path_played_function = modular_server_.createFunction(constants::get_last_audio_path_played_function_name);
  get_last_audio_path_played_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getLastAudioPathPlayedHandler));
  get_last_audio_path_played_function.setResultTypeString();

  modular_server::Function & get_position_function = modular_server_.createFunction(constants::get_position_function_name);
  get_position_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getPositionHandler));
  get_position_function.setResultTypeLong();

  modular_server::Function & get_length_function = modular_server_.createFunction(constants::get_length_function_name);
  get_length_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getLengthHandler));
  get_length_function.setResultTypeLong();

  modular_server::Function & get_percent_complete_function = modular_server_.createFunction(constants::get_percent_complete_function_name);
  get_percent_complete_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::getPercentCompleteHandler));
  get_percent_complete_function.setResultTypeLong();

  modular_server::Function & add_tone_pwm_function = modular_server_.createFunction(constants::add_tone_pwm_function_name);
  add_tone_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::addTonePwmHandler));
  add_tone_pwm_function.addParameter(frequency_parameter);
  add_tone_pwm_function.addParameter(speaker_parameter);
  add_tone_pwm_function.addParameter(delay_parameter);
  add_tone_pwm_function.addParameter(period_parameter);
  add_tone_pwm_function.addParameter(on_duration_parameter);
  add_tone_pwm_function.addParameter(count_parameter);
  add_tone_pwm_function.setResultTypeLong();

  modular_server::Function & add_tone_pwm_at_function = modular_server_.createFunction(constants::add_tone_pwm_at_function_name);
  add_tone_pwm_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::addTonePwmAtHandler));
  add_tone_pwm_at_function.addParameter(frequency_parameter);
  add_tone_pwm_at_function.addParameter(speaker_parameter);
  add_tone_pwm_at_function.addParameter(volume_parameter);
  add_tone_pwm_at_function.addParameter(delay_parameter);
  add_tone_pwm_at_function.addParameter(period_parameter);
  add_tone_pwm_at_function.addParameter(on_duration_parameter);
  add_tone_pwm_at_function.addParameter(count_parameter);
  add_tone_pwm_at_function.setResultTypeLong();

  modular_server::Function & start_tone_pwm_function = modular_server_.createFunction(constants::start_tone_pwm_function_name);
  start_tone_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::startTonePwmHandler));
  start_tone_pwm_function.addParameter(frequency_parameter);
  start_tone_pwm_function.addParameter(speaker_parameter);
  start_tone_pwm_function.addParameter(delay_parameter);
  start_tone_pwm_function.addParameter(period_parameter);
  start_tone_pwm_function.addParameter(on_duration_parameter);
  start_tone_pwm_function.setResultTypeLong();

  modular_server::Function & start_tone_pwm_at_function = modular_server_.createFunction(constants::start_tone_pwm_at_function_name);
  start_tone_pwm_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::startTonePwmAtHandler));
  start_tone_pwm_at_function.addParameter(frequency_parameter);
  start_tone_pwm_at_function.addParameter(speaker_parameter);
  start_tone_pwm_at_function.addParameter(volume_parameter);
  start_tone_pwm_at_function.addParameter(delay_parameter);
  start_tone_pwm_at_function.addParameter(period_parameter);
  start_tone_pwm_at_function.addParameter(on_duration_parameter);
  start_tone_pwm_at_function.setResultTypeLong();

  modular_server::Function & add_noise_pwm_function = modular_server_.createFunction(constants::add_noise_pwm_function_name);
  add_noise_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::addNoisePwmHandler));
  add_noise_pwm_function.addParameter(speaker_parameter);
  add_noise_pwm_function.addParameter(delay_parameter);
  add_noise_pwm_function.addParameter(period_parameter);
  add_noise_pwm_function.addParameter(on_duration_parameter);
  add_noise_pwm_function.addParameter(count_parameter);
  add_noise_pwm_function.setResultTypeLong();

  modular_server::Function & add_noise_pwm_at_function = modular_server_.createFunction(constants::add_noise_pwm_at_function_name);
  add_noise_pwm_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::addNoisePwmAtHandler));
  add_noise_pwm_at_function.addParameter(speaker_parameter);
  add_noise_pwm_at_function.addParameter(volume_parameter);
  add_noise_pwm_at_function.addParameter(delay_parameter);
  add_noise_pwm_at_function.addParameter(period_parameter);
  add_noise_pwm_at_function.addParameter(on_duration_parameter);
  add_noise_pwm_at_function.addParameter(count_parameter);
  add_noise_pwm_at_function.setResultTypeLong();

  modular_server::Function & start_noise_pwm_function = modular_server_.createFunction(constants::start_noise_pwm_function_name);
  start_noise_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::startNoisePwmHandler));
  start_noise_pwm_function.addParameter(speaker_parameter);
  start_noise_pwm_function.addParameter(delay_parameter);
  start_noise_pwm_function.addParameter(period_parameter);
  start_noise_pwm_function.addParameter(on_duration_parameter);
  start_noise_pwm_function.setResultTypeLong();

  modular_server::Function & start_noise_pwm_at_function = modular_server_.createFunction(constants::start_noise_pwm_at_function_name);
  start_noise_pwm_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::startNoisePwmAtHandler));
  start_noise_pwm_at_function.addParameter(speaker_parameter);
  start_noise_pwm_at_function.addParameter(volume_parameter);
  start_noise_pwm_at_function.addParameter(delay_parameter);
  start_noise_pwm_at_function.addParameter(period_parameter);
  start_noise_pwm_at_function.addParameter(on_duration_parameter);
  start_noise_pwm_at_function.setResultTypeLong();

  modular_server::Function & add_filtered_noise_pwm_function = modular_server_.createFunction(constants::add_filtered_noise_pwm_function_name);
  add_filtered_noise_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::addFilteredNoisePwmHandler));
  add_filtered_noise_pwm_function.addParameter(frequency_parameter);
  add_filtered_noise_pwm_function.addParameter(bandwidth_parameter);
  add_filtered_noise_pwm_function.addParameter(speaker_parameter);
  add_filtered_noise_pwm_function.addParameter(delay_parameter);
  add_filtered_noise_pwm_function.addParameter(period_parameter);
  add_filtered_noise_pwm_function.addParameter(on_duration_parameter);
  add_filtered_noise_pwm_function.addParameter(count_parameter);
  add_filtered_noise_pwm_function.setResultTypeLong();

  modular_server::Function & add_filtered_noise_pwm_at_function = modular_server_.createFunction(constants::add_filtered_noise_pwm_at_function_name);
  add_filtered_noise_pwm_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::addFilteredNoisePwmAtHandler));
  add_filtered_noise_pwm_at_function.addParameter(frequency_parameter);
  add_filtered_noise_pwm_at_function.addParameter(bandwidth_parameter);
  add_filtered_noise_pwm_at_function.addParameter(speaker_parameter);
  add_filtered_noise_pwm_at_function.addParameter(volume_parameter);
  add_filtered_noise_pwm_at_function.addParameter(delay_parameter);
  add_filtered_noise_pwm_at_function.addParameter(period_parameter);
  add_filtered_noise_pwm_at_function.addParameter(on_duration_parameter);
  add_filtered_noise_pwm_at_function.addParameter(count_parameter);
  add_filtered_noise_pwm_at_function.setResultTypeLong();

  modular_server::Function & start_filtered_noise_pwm_function = modular_server_.createFunction(constants::start_filtered_noise_pwm_function_name);
  start_filtered_noise_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::startFilteredNoisePwmHandler));
  start_filtered_noise_pwm_function.addParameter(frequency_parameter);
  start_filtered_noise_pwm_function.addParameter(bandwidth_parameter);
  start_filtered_noise_pwm_function.addParameter(speaker_parameter);
  start_filtered_noise_pwm_function.addParameter(delay_parameter);
  start_filtered_noise_pwm_function.addParameter(period_parameter);
  start_filtered_noise_pwm_function.addParameter(on_duration_parameter);
  start_filtered_noise_pwm_function.setResultTypeLong();

  modular_server::Function & start_filtered_noise_pwm_at_function = modular_server_.createFunction(constants::start_filtered_noise_pwm_at_function_name);
  start_filtered_noise_pwm_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::startFilteredNoisePwmAtHandler));
  start_filtered_noise_pwm_at_function.addParameter(frequency_parameter);
  start_filtered_noise_pwm_at_function.addParameter(bandwidth_parameter);
  start_filtered_noise_pwm_at_function.addParameter(speaker_parameter);
  start_filtered_noise_pwm_at_function.addParameter(volume_parameter);
  start_filtered_noise_pwm_at_function.addParameter(delay_parameter);
  start_filtered_noise_pwm_at_function.addParameter(period_parameter);
  start_filtered_noise_pwm_at_function.addParameter(on_duration_parameter);
  start_filtered_noise_pwm_at_function.setResultTypeLong();

  modular_server::Function & stop_pwm_function = modular_server_.createFunction(constants::stop_pwm_function_name);
  stop_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::stopPwmHandler));
  stop_pwm_function.addParameter(pwm_index_parameter);

  modular_server::Function & stop_all_pwm_function = modular_server_.createFunction(constants::stop_all_pwm_function_name);
  stop_all_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::stopAllPwmHandler));

  modular_server::Function & is_pulsing_function = modular_server_.createFunction(constants::is_pulsing_function_name);
  is_pulsing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&SoundTransmitter::isPulsingHandler));
  is_pulsing_function.setResultTypeBool();

  // Callbacks

}

bool SoundTransmitter::playPath(const char * path)
{
  char path_upper[constants::STRING_LENGTH_PATH];
  String(path).toUpperCase().toCharArray(path_upper,constants::STRING_LENGTH_PATH);

  bool sd_specified = false;
  char * sd_path;
  sd_path = strstr(path_upper,constants::sd_prefix);
  if (sd_path == path_upper)
  {
    sd_specified = true;
    // remove "/SD" from path
    sd_path = sd_path+strlen(constants::sd_prefix)-1;
  }

  // default to SD card if none specified
  if (!sd_specified)
  {
    sd_specified = true;
    sd_path = path_upper;
  }

  stop();
  bool playing = false;

  char * raw_ext = strstr(path_upper,constants::audio_ext_raw);
  if (raw_ext != NULL)
  {
    audio_type_playing_ = constants::RAW_TYPE;
    if (sd_specified)
    {
      playing = g_play_sd_raw.play(sd_path);
    }
  }

  if (!playing)
  {
    char * wav_ext = strstr(path_upper,constants::audio_ext_wav);
    if (wav_ext != NULL)
    {
      audio_type_playing_ = constants::WAV_TYPE;
      if (sd_specified)
      {
        playing = g_play_sd_wav.play(sd_path);
      }
    }
  }

  playing_ = playing;
  if (playing)
  {
    path_played_[0] = 0;
    strcpy(path_played_,path_upper);
  }
  return playing;
}

void SoundTransmitter::playTone(const size_t frequency,
                               const ConstantString * const speaker_ptr)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  playToneAt(frequency,speaker_ptr,volume);
}

void SoundTransmitter::playToneAt(const size_t frequency,
                                 const ConstantString * const speaker_ptr,
                                 const long volume)
{
  stop();
  if ((volume <= 0) || (frequency < constants::frequency_stop_threshold))
  {
    return;
  }
  audio_type_playing_ = constants::TONE_TYPE;
  if ((speaker_ptr == &constants::speaker_all) || (speaker_ptr == &constants::speaker_left))
  {
    g_tone_left.amplitude(0);
    g_tone_left.frequency(frequency);
    g_tone_left.amplitude(1);
  }
  if ((speaker_ptr == &constants::speaker_all) || (speaker_ptr == &constants::speaker_right))
  {
    g_tone_right.amplitude(0);
    g_tone_right.frequency(frequency);
    g_tone_right.amplitude(1);
  }
  setVolume(volume);
  playing_ = true;
}

void SoundTransmitter::playNoise(const ConstantString * const speaker_ptr)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  playNoiseAt(speaker_ptr,volume);
}

void SoundTransmitter::playNoiseAt(const ConstantString * const speaker_ptr,
                                  const long volume)
{
  stop();
  if (volume <= 0)
  {
    return;
  }
  audio_type_playing_ = constants::NOISE_TYPE;
  if ((speaker_ptr == &constants::speaker_all) || (speaker_ptr == &constants::speaker_left))
  {
    g_biquad_left.setCoefficients(constants::FILTER_STAGE_0,constants::allpass_filter_coefs);
    g_noise_left.amplitude(1);
  }
  if ((speaker_ptr == &constants::speaker_all) || (speaker_ptr == &constants::speaker_right))
  {
    g_biquad_right.setCoefficients(constants::FILTER_STAGE_0,constants::allpass_filter_coefs);
    g_noise_right.amplitude(1);
  }
  setVolume(volume);
  playing_ = true;
}

void SoundTransmitter::playFilteredNoise(const size_t frequency,
                                        const double bandwidth,
                                        const ConstantString * const speaker_ptr)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  playFilteredNoiseAt(frequency,bandwidth,speaker_ptr,volume);
}

void SoundTransmitter::playFilteredNoiseAt(const size_t frequency,
                                          const double bandwidth,
                                          const ConstantString * const speaker_ptr,
                                          const long volume)
{
  stop();
  if ((volume <= 0) || (frequency < constants::frequency_stop_threshold))
  {
    return;
  }
  audio_type_playing_ = constants::NOISE_TYPE;
  if ((speaker_ptr == &constants::speaker_all) || (speaker_ptr == &constants::speaker_left))
  {
    g_biquad_left.setBandpass(constants::FILTER_STAGE_0,frequency,bandwidth);
    g_noise_left.amplitude(1);
  }
  if ((speaker_ptr == &constants::speaker_all) || (speaker_ptr == &constants::speaker_right))
  {
    g_biquad_right.setBandpass(constants::FILTER_STAGE_0,frequency,bandwidth);
    g_noise_right.amplitude(1);
  }
  setVolume(volume);
  playing_ = true;
}

void SoundTransmitter::stop()
{
  if (playing_)
  {
    switch (audio_type_playing_)
    {
      case constants::RAW_TYPE:
      {
        g_play_sd_raw.stop();
        break;
      }
      case constants::WAV_TYPE:
      {
        g_play_sd_wav.stop();
        break;
      }
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

const char * SoundTransmitter::getLastAudioPathPlayed()
{
  return path_played_;
}

long SoundTransmitter::getPosition()
{
  long position = 0;
  if (playing_)
  {
    switch (audio_type_playing_)
    {
      case constants::RAW_TYPE:
      {
        position = g_play_sd_raw.positionMillis();
        break;
      }
      case constants::WAV_TYPE:
      {
        position = g_play_sd_wav.positionMillis();
        break;
      }
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
  return position;
}

long SoundTransmitter::getLength()
{
  long length = 0;
  if (playing_)
  {
    switch (audio_type_playing_)
    {
      case constants::RAW_TYPE:
      {
        length = g_play_sd_raw.lengthMillis();
        break;
      }
      case constants::WAV_TYPE:
      {
        length = g_play_sd_wav.lengthMillis();
        break;
      }
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
  return length;
}

int SoundTransmitter::getPercentComplete()
{
  long position = getPosition();
  long length = getLength();
  long percent_complete;
  if (length > 0)
  {
    percent_complete = (100*position)/length;
  }
  else if (isPlaying())
  {
    percent_complete = 0;
  }
  else
  {
    percent_complete = 100;
  }
  return percent_complete;
}

bool SoundTransmitter::codecEnabled()
{
  return codec_enabled_;
}

bool SoundTransmitter::pathIsAudio(const char * path)
{
  char path_upper[constants::STRING_LENGTH_PATH];
  String(path).toUpperCase().toCharArray(path_upper,constants::STRING_LENGTH_PATH);

  bool audio_path = false;
  for (unsigned int i=0;i<constants::AUDIO_EXT_COUNT;++i)
  {
    const char * audio_ext = constants::audio_exts[i];
    char * audio_ext_path = strstr(path_upper,audio_ext);
    if (audio_ext_path != NULL)
    {
      audio_path = true;
      break;
    }
  }
  return audio_path;
}

int SoundTransmitter::addTonePwm(const size_t frequency,
                                const ConstantString * const speaker_ptr,
                                const long delay,
                                const long period,
                                const long on_duration,
                                const long count)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  return addTonePwmAt(frequency,speaker_ptr,volume,delay,period,on_duration,count);
}

int SoundTransmitter::addTonePwmAt(const size_t frequency,
                                  const ConstantString * const speaker_ptr,
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
  pulse_info.speaker_ptr = speaker_ptr;
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

int SoundTransmitter::startTonePwm(const size_t frequency,
                                  const ConstantString * const speaker_ptr,
                                  const long delay,
                                  const long period,
                                  const long on_duration)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  return startTonePwmAt(frequency,speaker_ptr,volume,delay,period,on_duration);
}

int SoundTransmitter::startTonePwmAt(const size_t frequency,
                                    const ConstantString * const speaker_ptr,
                                    const long volume,
                                    const long delay,
                                    const long period,
                                    const long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  sound_transmitter::constants::PulseInfo pulse_info;
  pulse_info.frequency = frequency;
  pulse_info.speaker_ptr = speaker_ptr;
  pulse_info.volume = volume;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::playToneHandler),
                                                                         makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopHandler),
                                                                         delay,
                                                                         period,
                                                                         on_duration,
                                                                         index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

int SoundTransmitter::addNoisePwm(const ConstantString * const speaker_ptr,
                                 const long delay,
                                 const long period,
                                 const long on_duration,
                                 const long count)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  return addNoisePwmAt(speaker_ptr,volume,delay,period,on_duration,count);
}

int SoundTransmitter::addNoisePwmAt(const ConstantString * const speaker_ptr,
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
  pulse_info.speaker_ptr = speaker_ptr;
  pulse_info.volume = volume;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::playNoiseHandler),
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

int SoundTransmitter::startNoisePwm(const ConstantString * const speaker_ptr,
                                   const long delay,
                                   const long period,
                                   const long on_duration)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  return startNoisePwmAt(speaker_ptr,volume,delay,period,on_duration);
}

int SoundTransmitter::startNoisePwmAt(const ConstantString * const speaker_ptr,
                                     const long volume,
                                     const long delay,
                                     const long period,
                                     const long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  sound_transmitter::constants::PulseInfo pulse_info;
  pulse_info.speaker_ptr = speaker_ptr;
  pulse_info.volume = volume;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::playNoiseHandler),
                                                                         makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopHandler),
                                                                         delay,
                                                                         period,
                                                                         on_duration,
                                                                         index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

int SoundTransmitter::addFilteredNoisePwm(const size_t frequency,
                                         const double bandwidth,
                                         const ConstantString * const speaker_ptr,
                                         const long delay,
                                         const long period,
                                         const long on_duration,
                                         const long count)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  return addFilteredNoisePwmAt(frequency,bandwidth,speaker_ptr,volume,delay,period,on_duration,count);
}

int SoundTransmitter::addFilteredNoisePwmAt(const size_t frequency,
                                           const double bandwidth,
                                           const ConstantString * const speaker_ptr,
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
  pulse_info.bandwidth = bandwidth;
  pulse_info.speaker_ptr = speaker_ptr;
  pulse_info.volume = volume;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::playFilteredNoiseHandler),
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

int SoundTransmitter::startFilteredNoisePwm(const size_t frequency,
                                           const double bandwidth,
                                           const ConstantString * const speaker_ptr,
                                           const long delay,
                                           const long period,
                                           const long on_duration)
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);
  return startFilteredNoisePwmAt(frequency,bandwidth,speaker_ptr,volume,delay,period,on_duration);
}

int SoundTransmitter::startFilteredNoisePwmAt(const size_t frequency,
                                             const double bandwidth,
                                             const ConstantString * const speaker_ptr,
                                             const long volume,
                                             const long delay,
                                             const long period,
                                             const long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  sound_transmitter::constants::PulseInfo pulse_info;
  pulse_info.frequency = frequency;
  pulse_info.bandwidth = bandwidth;
  pulse_info.speaker_ptr = speaker_ptr;
  pulse_info.volume = volume;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::playFilteredNoiseHandler),
                                                                         makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopHandler),
                                                                         delay,
                                                                         period,
                                                                         on_duration,
                                                                         index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&SoundTransmitter::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

void SoundTransmitter::stopPwm(const int pwm_index)
{
  if (pwm_index < 0)
  {
    return;
  }
  if (indexed_pulses_.indexHasValue(pwm_index))
  {
    constants::PulseInfo pulse_info = indexed_pulses_[pwm_index];
    event_controller_.remove(pulse_info.event_id_pair);
  }
}

void SoundTransmitter::stopAllPwm()
{
  for (size_t i=0; i<constants::INDEXED_PULSES_COUNT_MAX; ++i)
  {
    stopPwm(i);
  }
}

bool SoundTransmitter::isPulsing()
{
  return pulsing_;
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
      case constants::RAW_TYPE:
      {
        playing_ = g_play_sd_raw.isPlaying();
        break;
      }
      case constants::WAV_TYPE:
      {
        playing_ = g_play_sd_wav.isPlaying();
        break;
      }
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

void SoundTransmitter::addDirectoryToResponse(File dir, const char * pwd)
{
  while (true)
  {
    File entry =  dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    char full_path[constants::STRING_LENGTH_PATH];
    full_path[0] = 0;
    strcat(full_path,pwd);
    strcat(full_path,entry.name());
    if (!entry.isDirectory())
    {
      bool audio_file = false;
      for (unsigned int i=0;i<constants::AUDIO_EXT_COUNT;++i)
      {
        if (strstr(full_path,constants::audio_exts[i]) != NULL)
        {
          audio_file = true;
        }
      }
      if (audio_file)
      {
        modular_server_.response().write(full_path);
      }
    }
    else
    {
      strcat(full_path,"/");
      addDirectoryToResponse(entry,full_path);
    }
    entry.close();
  }
}

void SoundTransmitter::setVolume(const long volume)
{
  double stereo_speaker_gain;
  modular_server_.property(constants::stereo_speaker_gain_property_name).getValue(stereo_speaker_gain);

  if (codecEnabled())
  {
    g_sgtl5000.volume(((double)volume*stereo_speaker_gain)/100.0);
  }

  double pcb_speaker_gain;
  modular_server_.property(constants::pcb_speaker_gain_property_name).getValue(pcb_speaker_gain);

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

void SoundTransmitter::setVolumeHandler()
{
  long volume;
  modular_server_.property(constants::volume_property_name).getValue(volume);

  setVolume(volume);
}

void SoundTransmitter::getAudioMemoryUsageHandler()
{
  modular_server_.response().returnResult(AudioMemoryUsage());
}

void SoundTransmitter::getAudioMemoryUsageMaxHandler()
{
  modular_server_.response().returnResult(AudioMemoryUsage());
}

void SoundTransmitter::resetAudioMemoryUsageMaxHandler()
{
  AudioMemoryUsageMaxReset();
}

void SoundTransmitter::getAudioProcessorUsageHandler()
{
  modular_server_.response().returnResult(AudioProcessorUsage());
}

void SoundTransmitter::getAudioProcessorUsageMaxHandler()
{
  modular_server_.response().returnResult(AudioProcessorUsage());
}

void SoundTransmitter::resetAudioProcessorUsageMaxHandler()
{
  AudioProcessorUsageMaxReset();
}

void SoundTransmitter::getSDCardInfoHandler()
{
  modular_server::Response & response = modular_server_.response();
  response.writeResultKey();
  response.beginObject();
  response.write("detected",sd_interface_.getDetected());
  response.write("type",sd_interface_.getType());
  response.write("formatted",sd_interface_.getFormatted());
  response.write("format",sd_interface_.getFormat());
  response.write("volume_size",sd_interface_.getVolumeSize());
  response.write("initialized",sd_interface_.getInitialized());
  response.endObject();
}

void SoundTransmitter::getAudioPathsHandler()
{
  File root = SD.open("/");
  modular_server::Response & response = modular_server_.response();
  response.writeResultKey();
  response.beginArray();
  addDirectoryToResponse(root,constants::sd_prefix);
  response.endArray();
}

void SoundTransmitter::playPathHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  const char * path;
  modular_server_.parameter(constants::audio_path_parameter_name).getValue(path);
  if (!pathIsAudio(path))
  {
    char error_str[constants::STRING_LENGTH_ERROR_MESSAGE];
    error_str[0] = 0;
    strcat(error_str,"Invalid audio path: ");
    strcat(error_str,path);
    response.returnError(error_str);
    return;
  }
  bool playing = playPath(path);
  if (!playing)
  {
    char error_str[constants::STRING_LENGTH_ERROR_MESSAGE];
    error_str[0] = 0;
    strcat(error_str,"Unable to find audio path: ");
    strcat(error_str,path);
    response.returnError(error_str);
  }
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
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  playTone(frequency,speaker_ptr);
}

void SoundTransmitter::playToneAtHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  playToneAt(frequency,speaker_ptr,volume);
}

void SoundTransmitter::playNoiseHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  playNoise(speaker_ptr);
}

void SoundTransmitter::playNoiseAtHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  playNoiseAt(speaker_ptr,volume);
}

void SoundTransmitter::playFilteredNoiseHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  double bandwidth;
  modular_server_.parameter(constants::bandwidth_parameter_name).getValue(bandwidth);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  playFilteredNoise(frequency,bandwidth,speaker_ptr);
}

void SoundTransmitter::playFilteredNoiseAtHandler()
{
  modular_server::Response & response = modular_server_.response();
  if (!codecEnabled())
  {
    response.returnError("No audio codec chip detected.");
    return;
  }
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  double bandwidth;
  modular_server_.parameter(constants::bandwidth_parameter_name).getValue(bandwidth);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  playFilteredNoiseAt(frequency,bandwidth,speaker_ptr,volume);
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

void SoundTransmitter::getLastAudioPathPlayedHandler()
{
  const char * last_path_played = getLastAudioPathPlayed();
  modular_server_.response().returnResult(last_path_played);
}

void SoundTransmitter::getPositionHandler()
{
  long position = getPosition();
  modular_server_.response().returnResult(position);
}

void SoundTransmitter::getLengthHandler()
{
  long length = getLength();
  modular_server_.response().returnResult(length);
}

void SoundTransmitter::getPercentCompleteHandler()
{
  long percent_complete = getPercentComplete();
  modular_server_.response().returnResult(percent_complete);
}

void SoundTransmitter::addTonePwmHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  int index = addTonePwm(frequency,speaker_ptr,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::addTonePwmAtHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  int index = addTonePwmAt(frequency,speaker_ptr,volume,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::startTonePwmHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  int index = startTonePwm(frequency,speaker_ptr,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::startTonePwmAtHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  int index = startTonePwmAt(frequency,speaker_ptr,volume,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::addNoisePwmHandler()
{
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  int index = addNoisePwm(speaker_ptr,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::addNoisePwmAtHandler()
{
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  int index = addNoisePwmAt(speaker_ptr,volume,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::startNoisePwmHandler()
{
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  int index = startNoisePwm(speaker_ptr,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::startNoisePwmAtHandler()
{
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  int index = startNoisePwmAt(speaker_ptr,volume,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::addFilteredNoisePwmHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  double bandwidth;
  modular_server_.parameter(constants::bandwidth_parameter_name).getValue(bandwidth);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  int index = addFilteredNoisePwm(frequency,bandwidth,speaker_ptr,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::addFilteredNoisePwmAtHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  double bandwidth;
  modular_server_.parameter(constants::bandwidth_parameter_name).getValue(bandwidth);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  int index = addFilteredNoisePwmAt(frequency,bandwidth,speaker_ptr,volume,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::startFilteredNoisePwmHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  double bandwidth;
  modular_server_.parameter(constants::bandwidth_parameter_name).getValue(bandwidth);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  int index = startFilteredNoisePwm(frequency,bandwidth,speaker_ptr,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::startFilteredNoisePwmAtHandler()
{
  long frequency;
  modular_server_.parameter(constants::frequency_parameter_name).getValue(frequency);
  double bandwidth;
  modular_server_.parameter(constants::bandwidth_parameter_name).getValue(bandwidth);
  const ConstantString * speaker_ptr;
  modular_server_.parameter(constants::speaker_parameter_name).getValue(speaker_ptr);
  long volume;
  modular_server_.parameter(constants::volume_parameter_name).getValue(volume);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  int index = startFilteredNoisePwmAt(frequency,bandwidth,speaker_ptr,volume,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void SoundTransmitter::stopPwmHandler()
{
  int pwm_index;
  modular_server_.parameter(constants::pwm_index_parameter_name).getValue(pwm_index);
  stopPwm(pwm_index);
}

void SoundTransmitter::stopAllPwmHandler()
{
  stopAllPwm();
}

void SoundTransmitter::isPulsingHandler()
{
  bool pulsing = isPulsing();
  modular_server_.response().returnResult(pulsing);
}

void SoundTransmitter::playToneHandler(int index)
{
  size_t frequency = indexed_pulses_[index].frequency;
  const ConstantString * speaker_ptr = indexed_pulses_[index].speaker_ptr;
  long volume = indexed_pulses_[index].volume;
  playToneAt(frequency,speaker_ptr,volume);
}

void SoundTransmitter::playNoiseHandler(int index)
{
  const ConstantString * speaker_ptr = indexed_pulses_[index].speaker_ptr;
  long volume = indexed_pulses_[index].volume;
  playNoiseAt(speaker_ptr,volume);
}

void SoundTransmitter::playFilteredNoiseHandler(int index)
{
  size_t frequency = indexed_pulses_[index].frequency;
  double bandwidth = indexed_pulses_[index].bandwidth;
  const ConstantString * speaker_ptr = indexed_pulses_[index].speaker_ptr;
  long volume = indexed_pulses_[index].volume;
  playFilteredNoiseAt(frequency,bandwidth,speaker_ptr,volume);
}

void SoundTransmitter::stopHandler(int index)
{
  stop();
}
