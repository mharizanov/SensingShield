// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

#define __SENSING__ 1
#include <JeeLib.h> 
#define freq RF12_868MHZ // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.


const int nodeID = 10;                                                  // emonTx RFM12B node ID
const int networkGroup = 210; 

// realPower|apparentPower|Vrms|Irms|powerFactor
typedef struct { int realPower, apparentPower, Vrms, powerFactor; double Irms;} PayloadTX;
PayloadTX emontx;

void setup()
{  
  Serial.begin(9600);
  rf12_initialize(nodeID, freq, networkGroup);                          // initialize RF
  emon1.voltage(3, 212.658, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(2, 60.61);       // Current: input pin, calibration.
}

void loop()
{
  emon1.calcVI(20,2000);         // Calculate all. No.of wavelengths, time-out
  //emon1.serialprint();           // Print out all variables
  // realPower|apparentPower|Vrms|Irms|powerFactor
  
  emontx.realPower = emon1.realPower;
  emontx.apparentPower = emon1.apparentPower;
  emontx.Vrms = emon1.Vrms;
  emontx.Irms = emon1.Irms;
  emontx.powerFactor = emon1.powerFactor*100;
    
  int i = 0; while (!rf12_canSend() && i<10) {rf12_recvDone(); i++;}
  rf12_sendStart(0, &emontx, sizeof emontx);
  Serial.println(emon1.realPower);
  Serial.println(emon1.Irms);
  delay(500);
  
}

