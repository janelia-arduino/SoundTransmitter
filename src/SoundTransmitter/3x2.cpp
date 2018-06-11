// ----------------------------------------------------------------------------
// 3x2.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "3x2.h"


#if defined(__MK20DX256__)

namespace sound_transmitter
{
namespace constants
{
const modular_server::HardwareInfo hardware_info =
{
  .name_ptr=&hardware_name,
  .part_number=1151,
  .version_major=1,
  .version_minor=0,
};

const size_t pcb_speaker_enable_pin = 6;

const bool sd_exists = false;
const size_t sd_card_cs_pin = 10;
const size_t sd_card_mosi_pin = 7;
const size_t sd_card_sck_pin = 14;

// Pins

// Units

// Properties

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
