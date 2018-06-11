#include <SoundTransmitter.h>


SoundTransmitter dev;

void setup()
{
  dev.setup();
  dev.startServer();
}

void loop()
{
  dev.update();
}
