// ----------------------------------------------------------------------------
// SoundTransmitter.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
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
#include "SoundTransmitter/SDInterface.h"


class SoundTransmitter : public ModularDeviceBase
{
public:
  SoundTransmitter();
  virtual void setup();

  SDInterface & getSDInterface();
  bool playPath(const char * path);
  void playTone(const size_t frequency,
                const ConstantString * const speaker_ptr);
  void playToneAt(const size_t frequency,
                  const ConstantString * const speaker_ptr,
                  const long volume);
  void playNoise(const ConstantString * const speaker_ptr);
  void playNoiseAt(const ConstantString * const speaker_ptr,
                   const long volume);
  void playFilteredNoise(const size_t frequency,
                         const double bandwidth,
                         const ConstantString * const speaker_ptr);
  void playFilteredNoiseAt(const size_t frequency,
                           const double bandwidth,
                           const ConstantString * const speaker_ptr,
                           const long volume);
  void stop();
  bool isPlaying();
  const char * getLastAudioPathPlayed();
  long getPosition();
  long getLength();
  int getPercentComplete();
  bool codecEnabled();
  bool pathIsAudio(const char * path);

  int addTonePwm(const size_t frequency,
                 const ConstantString * const speaker_ptr,
                 const long delay,
                 const long period,
                 const long on_duration,
                 const long count);
  int addTonePwmAt(const size_t frequency,
                   const ConstantString * const speaker_ptr,
                   const long volume,
                   const long delay,
                   const long period,
                   const long on_duration,
                   const long count);
  int startTonePwm(const size_t frequency,
                   const ConstantString * const speaker_ptr,
                   const long delay,
                   const long period,
                   const long on_duration);
  int startTonePwmAt(const size_t frequency,
                     const ConstantString * const speaker_ptr,
                     const long volume,
                     const long delay,
                     const long period,
                     const long on_duration);
  int addNoisePwm(const ConstantString * const speaker_ptr,
                  const long delay,
                  const long period,
                  const long on_duration,
                  const long count);
  int addNoisePwmAt(const ConstantString * const speaker_ptr,
                    const long volume,
                    const long delay,
                    const long period,
                    const long on_duration,
                    const long count);
  int startNoisePwm(const ConstantString * const speaker_ptr,
                    const long delay,
                    const long period,
                    const long on_duration);
  int startNoisePwmAt(const ConstantString * const speaker_ptr,
                      const long volume,
                      const long delay,
                      const long period,
                      const long on_duration);
  int addFilteredNoisePwm(const size_t frequency,
                          const double bandwidth,
                          const ConstantString * const speaker_ptr,
                          const long delay,
                          const long period,
                          const long on_duration,
                          const long count);
  int addFilteredNoisePwmAt(const size_t frequency,
                            const double bandwidth,
                            const ConstantString * const speaker_ptr,
                            const long volume,
                            const long delay,
                            const long period,
                            const long on_duration,
                            const long count);
  int startFilteredNoisePwm(const size_t frequency,
                            const double bandwidth,
                            const ConstantString * const speaker_ptr,
                            const long delay,
                            const long period,
                            const long on_duration);
  int startFilteredNoisePwmAt(const size_t frequency,
                              const double bandwidth,
                              const ConstantString * const speaker_ptr,
                              const long volume,
                              const long delay,
                              const long period,
                              const long on_duration);
  void stopPwm(const int pwm_index);
  void stopAllPwm();
  bool isPulsing();

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
  char path_played_[sound_transmitter::constants::STRING_LENGTH_PATH];
  SDInterface sd_interface_;
  bool pulsing_;

  void enableAudioCodec();
  void updatePlaying();
  void addDirectoryToResponse(File dir, const char * pwd);
  void setVolume(const long volume);

  // Handlers
  void setVolumeHandler();
  void getAudioMemoryUsageHandler();
  void getAudioMemoryUsageMaxHandler();
  void resetAudioMemoryUsageMaxHandler();
  void getAudioProcessorUsageHandler();
  void getAudioProcessorUsageMaxHandler();
  void resetAudioProcessorUsageMaxHandler();
  void getSDCardInfoHandler();
  void getAudioPathsHandler();
  void playPathHandler();
  void playToneHandler();
  void playToneAtHandler();
  void playNoiseHandler();
  void playNoiseAtHandler();
  void playFilteredNoiseHandler();
  void playFilteredNoiseAtHandler();
  void stopHandler();
  void isPlayingHandler();
  void getLastAudioPathPlayedHandler();
  void getPositionHandler();
  void getLengthHandler();
  void getPercentCompleteHandler();
  void addTonePwmHandler();
  void addTonePwmAtHandler();
  void startTonePwmHandler();
  void startTonePwmAtHandler();
  void addNoisePwmHandler();
  void addNoisePwmAtHandler();
  void startNoisePwmHandler();
  void startNoisePwmAtHandler();
  void addFilteredNoisePwmHandler();
  void addFilteredNoisePwmAtHandler();
  void startFilteredNoisePwmHandler();
  void startFilteredNoisePwmAtHandler();
  void stopPwmHandler();
  void stopAllPwmHandler();
  void isPulsingHandler();
  void playToneHandler(int index);
  void playNoiseHandler(int index);
  void playFilteredNoiseHandler(int index);
  void stopHandler(int index);

};

#endif
