#include <udpserial.h>

// Get local and secret wifi info.
#include "c:\wifi_definition.h"

UdpSerial udp(ssid, pass, server, serverPort, true);

void setup() 
{
  Serial.begin(9600);
  delay(1000);
  bool result = udp.begin();
  Serial.println(result ? "Success" : "Failure");
}

void loop() 
{
  static int count = 0;
  static long lastTime = millis()-1000;
  static char buff[255];
  static char readBuff[30];
  if (count < 100 && millis() >= lastTime + 1000)
  {
    count++;
    sprintf(buff,"Hello %d",count);
    Serial.println(buff);
    udp.write(buff, strlen(buff));
    lastTime += 1000;
  }
  if (udp.available())
  {
    udp.read(readBuff, 30);
    sprintf(buff, "ACK: %s", readBuff);
    udp.write(buff, strlen(buff));
  }
}
