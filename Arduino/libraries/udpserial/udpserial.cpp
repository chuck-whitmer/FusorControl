#include <udpserial.h>

bool UdpSerial::begin() 
{
  // check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) 
  {
	  if (debug) Serial.println("Communication with WiFi module failed!");
	  return false;
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) 
  {
	  if (debug)
	  {
	    Serial.print("Attempting to connect to WPA SSID: ");
	    Serial.println(ssid);
	  }
	// Connect to WPA/WPA2 network
	  status = WiFi.begin(ssid, pass);

	// wait 10 seconds for connection:
	  delay(10000);
	  //BlinkDelay(1,10000);
  }

  // you're connected now, so print out the data:
  if (debug)
  {
	  Serial.println("You're connected to the network");
	  printCurrentNet();
	  printWifiData();
  }
  return Udp.begin(localPort);
}

void UdpSerial::write(const char *buffer, long n)
{
	Udp.beginPacket(server, serverPort);
	Udp.write(buffer, n);
	Udp.endPacket();
}

int UdpSerial::available()
{
	if (charsToServe > 0)
		return charsToServe;
	charsToServe = Udp.parsePacket();
	if (charsToServe > 0)
	{
		Udp.read(packetBuffer,sizeof packetBuffer);
		nextChar = packetBuffer;
	}
	return charsToServe;
}

char UdpSerial::read()
{
	if (charsToServe > 0)
	{
		charsToServe--;
		return *nextChar++;
	}
	return 0;
}

void UdpSerial::printWifiData() 
{
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void UdpSerial::printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void UdpSerial::printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
	if (mac[i] < 16) {
	  Serial.print("0");
	}
	Serial.print(mac[i], HEX);
	if (i > 0) {
	  Serial.print(":");
	}
  }
  Serial.println();
}
