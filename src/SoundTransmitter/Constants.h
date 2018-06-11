// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef SOUND_TRANSMITTER_CONSTANTS_H
#define SOUND_TRANSMITTER_CONSTANTS_H
#include <ConstantVariable.h>
#include <ModularServer.h>

#include <EventController.h>


namespace sound_transmitter
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{PROPERTY_COUNT_MAX=3};
enum{PARAMETER_COUNT_MAX=2};
enum{FUNCTION_COUNT_MAX=4};
enum{CALLBACK_COUNT_MAX=1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

extern ConstantString hardware_name;
extern const modular_server::HardwareInfo hardware_info;

enum{AUDIO_MEMORY_BLOCK_COUNT=50};

enum audio_t
{
  TONE_TYPE,
  NOISE_TYPE,
};
extern const size_t pcb_speaker_enable_pin;

enum{EVENT_COUNT_MAX=8};
enum{INDEXED_PULSES_COUNT_MAX=4};

extern const int bad_index;

struct PulseInfo
{
  size_t frequency;
  long volume;
  EventIdPair event_id_pair;
};

extern const size_t frequency_stop_threshold;

extern const int pcb_speaker_reference;
extern const double pcb_speaker_channel_gain;
extern const double pcb_speaker_pre_gain;

// Pins

// Units
extern ConstantString hz_units;
extern ConstantString percent_units;
extern ConstantString ms_units;

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern ConstantString trigger_tone_frequency_property_name;
extern const long frequency_min;
extern const long frequency_max;
extern const long frequency_default;

extern ConstantString trigger_tone_volume_property_name;
extern const long volume_min;
extern const long volume_max;
extern const long volume_default;

extern ConstantString trigger_tone_duration_property_name;
extern const long duration_min;
extern const long duration_max;
extern const long duration_default;

// Parameters
extern ConstantString frequency_parameter_name;

extern ConstantString volume_parameter_name;

// Functions
extern ConstantString play_tone_function_name;
extern ConstantString play_noise_function_name;
extern ConstantString stop_function_name;
extern ConstantString is_playing_function_name;

// Callbacks
extern ConstantString trigger_tone_callback_name;

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
