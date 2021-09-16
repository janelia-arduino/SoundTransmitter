// ----------------------------------------------------------------------------
// SoundTransmitter.h
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef SOUND_TRANSMITTER_H
#define SOUND_TRANSMITTER_H
#include <ArduinoJson.h>
#include <JsonStream.h>
#include <Array.h>
#include <Vector.h>
#include <ConstantVariable.h>
#include <Functor.h>

#include <IndexedContainer.h>
#include <EventController.h>

#include <Audio.h>

#include <ModularServer.h>
#include <ModularDeviceBase.h>

#include "SoundTransmitter/Constants.h"


class SoundTransmitter : public ModularDeviceBase
{
public:
  SoundTransmitter();
  virtual void setup();

  void playTone(size_t frequency,
    long volume);
  void playNoise(long volume);
  void stop();
  bool isPlaying();

  bool codecEnabled();
  int addTonePwm(size_t frequency,
    long volume,
    long delay,
    long period,
    long on_duration,
    long count);

  // Handlers
  virtual void startPwmHandler(int index);
  virtual void stopPwmHandler(int index);

private:
  modular_server::Pin pins_[sound_transmitter::constants::PIN_COUNT_MAX];

  modular_server::Property properties_[sound_transmitter::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[sound_transmitter::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[sound_transmitter::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[sound_transmitter::constants::CALLBACK_COUNT_MAX];

  EventController<sound_transmitter::constants::EVENT_COUNT_MAX> event_controller_;

  IndexedContainer<sound_transmitter::constants::PulseInfo,
    sound_transmitter::constants::INDEXED_PULSES_COUNT_MAX> indexed_pulses_;

  bool codec_enabled_;
  sound_transmitter::constants::audio_t audio_type_playing_;
  bool playing_;
  bool pulsing_;

  void enableAudioCodec();
  void updatePlaying();
  void setVolume(long volume);

  // Handlers
  void playToneHandler();
  void playNoiseHandler();
  void stopHandler();
  void isPlayingHandler();
  void triggerToneHandler(modular_server::Pin * pin_ptr);
  void playToneHandler(int index);
  void playNoiseHandler(int index);
  void stopHandler(int index);

};

#endif
