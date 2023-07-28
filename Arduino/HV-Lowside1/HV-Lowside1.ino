//
// Fusor project code for HV-lowside Arduino
// 
// Adafruit - Mega 2560 => Transitioning to Giga Wifi R1
//

#define DEVICE_GIGA
// Substitute UDP over wifi for serial.
#define UDPCOMM

#include <fusor.h>
#include <stat.h>
//#include <LinearAlgebra.h>
#include <SinFit60Hz.h>

#define variacAdcPin 0
// #define nstAdcPin 5
// #define cwAdcPin 6
// #define cwCurrentAdcPin 1

SinFit60Hz variacOutputFitter = SinFit60Hz();
// SinFit60Hz nstOutputFitter = SinFit60Hz();
// Fusor::stat cwOutput;
// Fusor::stat cwCurrent;

long nextDisplayUpdate;

const int captureCycles = 6;
const long captureTimeUs = (long)(1.0 / 60.0 * captureCycles * 1000000.0);

const float VARIAC_R1 = 10.83 * 1000;
const float VARIAC_R2 = 88.6 * 1000;
const float VARIAC_R3 = 3.23 * 1000000;

// const float NST_R1 = 10.82 * 1000;
// const float NST_R2 = 89.3 * 1000;
// const float NST_R3 = 200 * 1000000;


float DividerMultiplier(float r1, float r2, float rS, float rL)
{
  float r3 = rS + rL;
  float rr123 = r1*r2 + r1*r3 + r2*r3;
  float rr12s = r1*r2 + r1*rS + r2*rS;
  return rr123 / rr12s;
}

// float cwOffset = 1.017; // DividerOffset(330.0, 82.0, 10e3, 400e6, 5.0); // Was 0.995, but measured 1.017
// float cwMultiplier = DividerMultiplier(330.0, 82.0, 10e3, 400e6) / 1000.0; // Make it KV.
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
#else
const float vRange = 1.1;
#endif

void setup(){
  fusorInit("HV-LOWSIDE"); //Fusor device name, variables, num variables
  fusorAddVariable("variac_rms", FUSOR_VARTYPE_FLOAT);
  // fusorAddVariable("nst_rms", FUSOR_VARTYPE_FLOAT);
  // fusorAddVariable("cw_avg", FUSOR_VARTYPE_FLOAT);
  // fusorAddVariable("cwc_avg", FUSOR_VARTYPE_FLOAT);
  fusorIdentify();
  #ifndef DEVICE_GIGA
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

  // float nstReading = readVoltage(nstAdcPin);
  // nstOutputFitter.Accumulate(micros(), v(nstReading, NST_R1, NST_R2, NST_R3)/1000); //in KV 

  // #define HACK_OFFSET 0.75
  // float cwReading = readVoltage(cwAdcPin);
  // cwOutput.accumulate((cwReading - cwOffset) * cwMultiplier - HACK_OFFSET); // In KV.

  // Read the CW current
  // Measured as voltage over 100 Ohm resistor
  // float cwCurrentReading = readVoltage(cwCurrentAdcPin); // readConstantTime(cwCurrentAdcPin, 0, 1000.0/currentResistor); // in mA
  // cwCurrent.accumulate((cwCurrentReading) * (1000.0/currentResistor));
}

void UpdateDisplay()
{
  double variacA, variacB, variacC;
  variacOutputFitter.SolveFit(variacA, variacB, variacC);
  
  float variacRMS = rms(variacA, variacB);
  variacOutputFitter.Reset();
  
  // double nstA, nstB, nstC;
  // nstOutputFitter.SolveFit(nstA, nstB, nstC);
  
  // float nstRMS = rms(nstA, nstB);
  // nstOutputFitter.Reset();

  // float cwAverage = cwOutput.average();
  // float cwcAverage = cwCurrent.average();
  // cwOutput.Reset();
  // cwCurrent.Reset();

  fusorSetFloatVariable("variac_rms", variacRMS);
  // fusorSetFloatVariable("nst_rms", nstRMS);
  // fusorSetFloatVariable("cw_avg", cwAverage);
  // fusorSetFloatVariable("cwc_avg", cwcAverage);
}
