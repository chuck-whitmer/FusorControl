//
// Fusor project code for HV-lowside1 Arduino - Just Variac
// 
// Adafruit - Mega 2560 => Transitioning to Giga Wifi R1 and Uno R4 Wifi
//

// #define DEVICE_GIGA
#define DEVICE_UNO_WIFI
// #define DEVICE_MEGA

// Substitute UDP over wifi for serial.
#define UDPCOMM

#include <fusor.h>
#include <stat.h>
#include <SinFit60Hz.h>

#define variacAdcPin 0

SinFit60Hz variacOutputFitter = SinFit60Hz();

long nextDisplayUpdate;

const int captureCycles = 6;
const long captureTimeUs = (long)(1.0 / 60.0 * captureCycles * 1000000.0);

const float VARIAC_R1 = 10.83 * 1000;
const float VARIAC_R2 = 88.6 * 1000;
const float VARIAC_R3 = 3.23 * 1000000;

float DividerMultiplier(float r1, float r2, float rS, float rL)
{
  float r3 = rS + rL;
  float rr123 = r1*r2 + r1*r3 + r2*r3;
  float rr12s = r1*r2 + r1*rS + r2*rS;
  return rr123 / rr12s;
}

const float currentResistor = 100; // 100 Ohm

float f(float r1, float r2, float r3)
{
  return (r1*r2)/(r1*r3+r1*r2+r2*r3);
}

float v(float a, float r1, float r2, float r3)
{
  return (a - 5*f(r1,r3,r2))/f(r1,r2,r3);
}

float rms(float a, float b)
{
  return sqrt(a*a+b*b)*0.707107;
}

#ifdef DEVICE_GIGA
  const float vRange = 3.3;
#elif defined(DEVICE_UNO_WIFI)
  const float vRange = 5.0;
#elif defined(DEVICE_MEGA)
  const float vRange = 1.1;
#endif

void setup(){
  fusorInit("HV-LOWSIDE1"); //Fusor device name, variables, num variables
  fusorAddVariable("variac_rms", FUSOR_VARTYPE_FLOAT);
  fusorIdentify();

  #ifdef DEVICE_GIGA
  #elif defined(DEVICE_UNO_WIFI)
  #elif defined(DEVICE_MEGA)
    analogReference(INTERNAL1V1); // Mega ADCs compare to 1.1v
  #endif

  nextDisplayUpdate = millis() + 1000;

  FUSOR_LED_ON();
  delay(200);
  FUSOR_LED_OFF();
}

void loop() {
  fusorLoop();
  long endTime = micros()+captureTimeUs;
  do {
    updateAll();
  } while (micros() < endTime);
  UpdateDisplay();
}

float readVoltage(int pin)
{
  return analogRead(pin)/1023.0*vRange;
}

void updateAll() {
  float variacReading = readVoltage(variacAdcPin);
  variacOutputFitter.Accumulate(micros(), v(variacReading, VARIAC_R1, VARIAC_R2, VARIAC_R3));   
}

void UpdateDisplay()
{
  double variacA, variacB, variacC;
  variacOutputFitter.SolveFit(variacA, variacB, variacC);
  
  float variacRMS = rms(variacA, variacB);
  variacOutputFitter.Reset();
  
  fusorSetFloatVariable("variac_rms", variacRMS);
}
