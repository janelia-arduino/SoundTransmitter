// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Constants.h"


namespace sound_transmitter
{
namespace constants
{
CONSTANT_STRING(device_name,"sound_transmitter");

CONSTANT_STRING(firmware_name,"SoundTransmitter");
// Use semantic versioning http://semver.org/
const modular_server::FirmwareInfo firmware_info =
{
  .name_ptr=&firmware_name,
  .version_major=1,
  .version_minor=0,
  .version_patch=0,
};

CONSTANT_STRING(hardware_name,"sound_transmitter");

const int bad_index = -1;

const size_t frequency_stop_threshold = 200;

const int pcb_speaker_reference = INTERNAL;
const double pcb_speaker_channel_gain = 0.5;
const double pcb_speaker_pre_gain = 0.5;

// Pins

// Units
CONSTANT_STRING(hz_units,"Hz");
CONSTANT_STRING(ms_units,"ms");
CONSTANT_STRING(percent_units,"%");

// Properties
CONSTANT_STRING(trigger_tone_frequency_property_name,"triggerToneFrequency");
const long frequency_min = 0;
const long frequency_max = 22000;
const long frequency_default = 5000;

CONSTANT_STRING(trigger_tone_volume_property_name,"triggerToneVolume");
const long volume_min = 0;
const long volume_max = 100;
const long volume_default = 25;

CONSTANT_STRING(trigger_tone_duration_property_name,"triggerToneDuration");
const long duration_min = 10;
const long duration_max = 10000;
const long duration_default = 1000;

// Parameters
CONSTANT_STRING(frequency_parameter_name,"frequency");

CONSTANT_STRING(volume_parameter_name,"volume");

// Functions
CONSTANT_STRING(play_tone_function_name,"playTone");
CONSTANT_STRING(play_noise_function_name,"playNoise");
CONSTANT_STRING(stop_function_name,"stop");
CONSTANT_STRING(is_playing_function_name,"isPlaying");

// Callbacks
CONSTANT_STRING(trigger_tone_callback_name,"triggerTone");

// Errors
}
}
