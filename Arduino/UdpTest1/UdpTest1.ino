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

void loop() {
  // put your main code here, to run repeatedly:

}
