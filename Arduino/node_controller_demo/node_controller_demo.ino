#include <SPI.h>
#include <LoRa.h>
#include "LowPower.h"
#include <math.h>

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/

uint8_t nodeId = 10;          // Node ID
uint8_t gatewayId = 214;      // Selected gateway ID to communicate
uint8_t generalId = 0;        // Node ID for gateway to access every node
uint8_t checkSum = 200;       // Check sum code, must be same with corresponding gateway and its sensor unit group

uint8_t transmitData[10] = {0,0,0,0,0,0,0,0,0,0};    // Transmit data: Byte array data to send using LoRa wireless
const uint8_t transmitLen = sizeof(transmitData);    // Transmit data length

uint8_t arrayData[6] = {1,2,3,4,5,6};                // measurement data from current period
const uint8_t arrayLen = sizeof(arrayData);          // measurement data length

uint8_t batVol;                 // battery voltage data

/* Packet data structure to send
|---------|------------|------------|-------------|--------|--------|--------|--------|--------|--------|
| Byte 0  | Byte 1     | Byte 2     | Byte 3      | Byte 4 | Byte 5 | Byte 6 | Byte 7 | Byte 8 | Byte 9 |
|---------|------------|------------|-------------|--------|--------|--------|--------|--------|--------|
| Node ID | Gateway ID | Check Code | Bat Voltage | data0  | data1  | data2  | data3  | data4  | data5  |
|---------|------------|------------|-------------|--------|--------|--------|--------|--------|--------|
*/

/*
============================================================================================================================
|| TIME AND PERIOD SETUP
============================================================================================================================
*/

// Period time
uint32_t tPeriod = 10000;

// Sleep phase time
uint32_t tSleep;

// Start time
uint32_t tStart;

// sleep time step. default is 125 ms
// adjust to match to the watchdog timer
uint16_t tStepNom = 120;

/*
============================================================================================================================
|| WIRELESS SETUP
============================================================================================================================
*/
const uint8_t csPin = 10;         // LoRa radio chip select
const uint8_t resetPin = 9;       // LoRa radio reset
const uint8_t irqPin = 2;         // change for your board; must be a hardware interrupt pin
const uint8_t rxpin = 7;          // setup only for e-byte E-19 LoRa module. Must set to LOW when transmit and HIGH when receive
const uint8_t txpin = 8;          // setup only for e-byte E-19 LoRa module. Must set to HIGH when transmit and LOW when receive

uint32_t LoRaFreq = 433E6;        // LoRa wireless communication frequency

/*
============================================================================================================================
|| SENSORS SETUP
============================================================================================================================
*/

int16_t batCoef = 980;          // analog reading when battery voltage is 4.2V (default 999)
int8_t batPin = A6;             // analog pin number to measure battery voltage

/*
============================================================================================================================
|| SETUP FUNCTION
============================================================================================================================
*/

void setup()
{

  // set to low all unused pin of ATmega328P
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  // reset the start time
  tStart = millis();

}

/*
============================================================================================================================
|| MAIN FUNCTION
============================================================================================================================
*/

void loop()
{

  // measure battery voltage
  measureBatFunction(&batVol);

  // prepare data to transmit
  transmitDataFunction(batVol, arrayData, arrayLen);
  
  // transmit data
  transmitFunction();

  // begin sleep phase when measure timeout is reached
  tSleep = tPeriod + millis() - tStart;
  sleepFunction(tSleep);
  
  // reset the start time
  tStart = millis();

}

/*
============================================================================================================================
|| MEASURE FUNCTION
============================================================================================================================
*/

void measureBatFunction(uint8_t* batteryVoltage){

  // calculate battery voltage
  double batV = 4.2 * analogRead(batPin) / batCoef;
  *batteryVoltage = round(batV * 100) - 200;
  
}

/*
============================================================================================================================
|| SLEEP FUNCTION
============================================================================================================================
*/

void sleepFunction(uint32_t sleepTime){

  // put LoRa wireless to sleep mode
  LoRa.end();

  // put AtMega328P to power down mode
  uint16_t stime = 0;
  while(stime < sleepTime){
    LowPower.powerDown(SLEEP_120MS, ADC_ON, BOD_OFF);
    stime += tStepNom;
  }
  
}

/*
============================================================================================================================
|| TRANSMIT FUNCTION
============================================================================================================================
*/

void transmitFunction(){

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  //Begin Lora transmission
  LoRa.begin(LoRaFreq);
  
  // setup only for e-byte E-19 LoRa module
  pinMode(rxpin, OUTPUT);
  pinMode(txpin, OUTPUT);
  digitalWrite(rxpin, LOW);
  digitalWrite(txpin, HIGH);
  
  // send packet via LoRa
  LoRa.beginPacket();
  LoRa.write(transmitData, transmitLen);
  LoRa.endPacket();

}

void transmitDataFunction(uint8_t batteryVoltage, uint8_t data[], uint8_t dataLen){

  // asign id to transmit data
  transmitData[0] = nodeId;
  transmitData[1] = gatewayId;

  // asign battery voltage to transmit data
  transmitData[3] = batteryVoltage;

  // asign measurement data to transmit data
  for(uint8_t i=0; i<dataLen; i++){
    transmitData[i+transmitLen-dataLen] = data[i];
  }

  // calculate checksum code
  transmitData[2] = 0;
  uint8_t sumData = 0;
  for(uint8_t i=0; i<transmitLen ;i++){
    sumData = sumData + transmitData[i];
  }
  // asign checksum code to transmit data
  transmitData[2] = checkSum - sumData;
}
