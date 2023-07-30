#include <SPI.h>
// Giga-specific WiFi.h
#include <WiFi.h>
#include <WiFiUdp.h>

class UdpSerial 
{
	static size_t constexpr maxSsidLength = 20;
	static size_t constexpr maxPassLength = 20;
	char ssid[maxSsidLength];    // network SSID (name)
	char pass[maxPassLength];    // network password (use for WPA, or use as key for WEP)
	IPAddress server;
	long serverPort;
	bool debug;
	
	int status = WL_IDLE_STATUS;     // the WiFi radio's status
	unsigned int localPort = 2390;   // local port to listen on
	char packetBuffer[255]; //buffer to hold incoming packet
	int charsToServe = 0;
	char *nextChar = NULL;
	WiFiUDP Udp;
public:
	UdpSerial(const char *_ssid, const char* _pass, 
		IPAddress _server, long _serverPort, bool debug=false) : server(_server), serverPort(_serverPort), debug(debug)
	{
		// Save connection info for later use.
		// Note that constructors don't return errors, so this must succeed.
		// If too-long parameters are passed in, we will truncate and fail later.
		strncpy(ssid, _ssid, maxSsidLength-1);
		ssid[maxSsidLength-1] = '\0';
		strncpy(pass, _pass, maxPassLength-1);
		ssid[maxPassLength-1] = '\0';
	}
	
	bool begin();
	void write(const uint8_t *buffer, long n);
	int available();
	char read();
	int read(char *buffer, int n);

	void printWifiData();
	void printCurrentNet();
	void printMacAddress(byte mac[]);
};
