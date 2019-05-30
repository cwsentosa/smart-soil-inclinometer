#include <SPI.h>
#include <LoRa.h>
//#include "LowPower.h"
#include <math.h>

// Define the instruction code for sending messages
#define SEND 0

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/
uint8_t msg_id = 0;
const uint8_t instruction_code = SEND;
uint8_t nodeId = 10;     // Node ID
uint8_t gatewayId = 214; // Selected gateway ID to communicate
uint8_t generalId = 0;   // Node ID for gateway to access every node
uint8_t checkSum = 200;  // Check sum code, must be same with corresponding gateway and its sensor unit group

uint8_t arrayData_1[6] = {1, 1, 2, 2, 3, 3};       // measurement data from current period
uint8_t arrayData_2[6] = {6, 6, 5, 5, 4, 4};       // measurement data from current period
uint8_t arrayData_3[6] = {9, 9, 8, 8, 7, 7};       // measurement data from current period
uint8_t arrayData_4[6] = {10, 10, 11, 11, 12, 12}; // measurement data from current period
uint8_t arrayData_5[6] = {13, 13, 14, 14, 15, 15}; // measurement data from current period
const uint8_t arrayLen_1 = sizeof(arrayData_1);    // measurement data length
const uint8_t arrayLen_2 = sizeof(arrayData_2);    // measurement data length
const uint8_t arrayLen_3 = sizeof(arrayData_3);    // measurement data length
const uint8_t arrayLen_4 = sizeof(arrayData_4);    // measurement data length
const uint8_t arrayLen_5 = sizeof(arrayData_5);    // measurement data length

uint8_t batVol = 100; // battery voltage data

uint8_t transmitData[42] = {
    msg_id, instruction_code,
    gatewayId, gatewayId,
    nodeId,
    batVol,
    checkSum,
    1,
    arrayData_1[0], arrayData_1[1],
    arrayData_1[2], arrayData_1[3],
    arrayData_1[4], arrayData_1[5],
    2,
    arrayData_2[0], arrayData_2[1],
    arrayData_2[2], arrayData_2[3],
    arrayData_2[4], arrayData_2[5],
    3,
    arrayData_3[0], arrayData_3[1],
    arrayData_3[2], arrayData_3[3],
    arrayData_3[4], arrayData_3[5],
    4,
    arrayData_4[0], arrayData_4[1],
    arrayData_4[2], arrayData_4[3],
    arrayData_4[4], arrayData_4[5],
    5,
    arrayData_5[0], arrayData_5[1],
    arrayData_5[2], arrayData_5[3],
    arrayData_5[4], arrayData_5[5]};              // Transmit data: Byte array data to send using LoRa wireless
const uint8_t transmitLen = sizeof(transmitData); // Transmit data length

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
const uint8_t csPin = 10;   // LoRa radio chip select
const uint8_t resetPin = 9; // LoRa radio reset
const uint8_t irqPin = 2;   // change for your board; must be a hardware interrupt pin
const uint8_t rxpin = 7;    // setup only for e-byte E-19 LoRa module. Must set to LOW when transmit and HIGH when receive
const uint8_t txpin = 8;    // setup only for e-byte E-19 LoRa module. Must set to HIGH when transmit and LOW when receive

uint32_t LoRaFreq = 915E6; // LoRa wireless communication frequency

/*
============================================================================================================================
|| SENSORS SETUP
============================================================================================================================
*/

int16_t batCoef = 980; // analog reading when battery voltage is 4.2V (default 999)
int8_t batPin = A6;    // analog pin number to measure battery voltage

/*
============================================================================================================================
|| SETUP FUNCTION
============================================================================================================================
*/

void setup()
{
  Serial.begin(9600);
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
  //  measureBatFunction(&batVol);

  // prepare data to transmit
  //  transmitDataFunction(batVol);

  // transmit data
  transmitFunction();
  delay(3000);
  //  // begin sleep phase when measure timeout is reached
  //  tSleep = tPeriod + millis() - tStart;
  //  sleepFunction(tSleep);
  //
  //  // reset the start time
  //  tStart = millis();
}

/*
============================================================================================================================
|| MEASURE FUNCTION
============================================================================================================================
*/

void measureBatFunction(uint8_t *batteryVoltage)
{

  // calculate battery voltage
  double batV = 4.2 * analogRead(batPin) / batCoef;
  *batteryVoltage = round(batV * 100) - 200;
}

/*
============================================================================================================================
|| SLEEP FUNCTION
============================================================================================================================
*/

void sleepFunction(uint32_t sleepTime)
{

  // put LoRa wireless to sleep mode
  LoRa.end();

  // put AtMega328P to power down mode
  uint16_t stime = 0;
  while (stime < sleepTime)
  {
    //    LowPower.powerDown(SLEEP_120MS, ADC_ON, BOD_OFF);
    stime += tStepNom;
  }
}

/*
============================================================================================================================
|| TRANSMIT FUNCTION
============================================================================================================================
*/

void transmitFunction()
{

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  //Begin Lora transmission
  LoRa.begin(LoRaFreq);

  // setup only for e-byte E-19 LoRa module
  pinMode(rxpin, OUTPUT);
  pinMode(txpin, OUTPUT);
  digitalWrite(rxpin, LOW);
  digitalWrite(txpin, HIGH);
  Serial.println("Data to Send:");

  for (int i = 0; i < transmitLen; i++)
  {
    Serial.print(transmitData[i]);
    Serial.print(", ");
  }
  Serial.println("");

  Serial.println("Begin Transmission");

  // send packet via LoRa
  LoRa.beginPacket();
  LoRa.write(transmitData, transmitLen);
  LoRa.endPacket();

  Serial.println("End of Transmission.");
  transmitData[0] = transmitData[0] + 1;
}
#include <SPI.h>
#include <LoRa.h>
//#include "LowPower.h"
#include <math.h>

// Define the instruction code for sending messages
#define SEND 0

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/
uint8_t msg_id = 0;
const uint8_t instruction_code = SEND;
uint8_t nodeId = 10;     // Node ID
uint8_t gatewayId = 214; // Selected gateway ID to communicate
uint8_t generalId = 0;   // Node ID for gateway to access every node
uint8_t checkSum = 200;  // Check sum code, must be same with corresponding gateway and its sensor unit group

uint8_t arrayData_1[6] = {1, 1, 2, 2, 3, 3};       // measurement data from current period
uint8_t arrayData_2[6] = {6, 6, 5, 5, 4, 4};       // measurement data from current period
uint8_t arrayData_3[6] = {9, 9, 8, 8, 7, 7};       // measurement data from current period
uint8_t arrayData_4[6] = {10, 10, 11, 11, 12, 12}; // measurement data from current period
uint8_t arrayData_5[6] = {13, 13, 14, 14, 15, 15}; // measurement data from current period
const uint8_t arrayLen_1 = sizeof(arrayData_1);    // measurement data length
const uint8_t arrayLen_2 = sizeof(arrayData_2);    // measurement data length
const uint8_t arrayLen_3 = sizeof(arrayData_3);    // measurement data length
const uint8_t arrayLen_4 = sizeof(arrayData_4);    // measurement data length
const uint8_t arrayLen_5 = sizeof(arrayData_5);    // measurement data length

uint8_t batVol = 100; // battery voltage data

uint8_t transmitData[42] = {
    msg_id, instruction_code,
    gatewayId, gatewayId,
    nodeId,
    batVol,
    checkSum,
    1,
    arrayData_1[0], arrayData_1[1],
    arrayData_1[2], arrayData_1[3],
    arrayData_1[4], arrayData_1[5],
    2,
    arrayData_2[0], arrayData_2[1],
    arrayData_2[2], arrayData_2[3],
    arrayData_2[4], arrayData_2[5],
    3,
    arrayData_3[0], arrayData_3[1],
    arrayData_3[2], arrayData_3[3],
    arrayData_3[4], arrayData_3[5],
    4,
    arrayData_4[0], arrayData_4[1],
    arrayData_4[2], arrayData_4[3],
    arrayData_4[4], arrayData_4[5],
    5,
    arrayData_5[0], arrayData_5[1],
    arrayData_5[2], arrayData_5[3],
    arrayData_5[4], arrayData_5[5]};              // Transmit data: Byte array data to send using LoRa wireless
const uint8_t transmitLen = sizeof(transmitData); // Transmit data length

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
const uint8_t csPin = 10;   // LoRa radio chip select
const uint8_t resetPin = 9; // LoRa radio reset
const uint8_t irqPin = 2;   // change for your board; must be a hardware interrupt pin
const uint8_t rxpin = 7;    // setup only for e-byte E-19 LoRa module. Must set to LOW when transmit and HIGH when receive
const uint8_t txpin = 8;    // setup only for e-byte E-19 LoRa module. Must set to HIGH when transmit and LOW when receive

uint32_t LoRaFreq = 915E6; // LoRa wireless communication frequency

/*
============================================================================================================================
|| SENSORS SETUP
============================================================================================================================
*/

int16_t batCoef = 980; // analog reading when battery voltage is 4.2V (default 999)
int8_t batPin = A6;    // analog pin number to measure battery voltage

/*
============================================================================================================================
|| SETUP FUNCTION
============================================================================================================================
*/

void setup()
{
  Serial.begin(9600);
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
  //  measureBatFunction(&batVol);

  // prepare data to transmit
  //  transmitDataFunction(batVol);

  // transmit data
  transmitFunction();
  delay(3000);
  //  // begin sleep phase when measure timeout is reached
  //  tSleep = tPeriod + millis() - tStart;
  //  sleepFunction(tSleep);
  //
  //  // reset the start time
  //  tStart = millis();
}

/*
============================================================================================================================
|| MEASURE FUNCTION
============================================================================================================================
*/

void measureBatFunction(uint8_t *batteryVoltage)
{

  // calculate battery voltage
  double batV = 4.2 * analogRead(batPin) / batCoef;
  *batteryVoltage = round(batV * 100) - 200;
}

/*
============================================================================================================================
|| SLEEP FUNCTION
============================================================================================================================
*/

void sleepFunction(uint32_t sleepTime)
{

  // put LoRa wireless to sleep mode
  LoRa.end();

  // put AtMega328P to power down mode
  uint16_t stime = 0;
  while (stime < sleepTime)
  {
    //    LowPower.powerDown(SLEEP_120MS, ADC_ON, BOD_OFF);
    stime += tStepNom;
  }
}

/*
============================================================================================================================
|| TRANSMIT FUNCTION
============================================================================================================================
*/

void transmitFunction()
{

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  //Begin Lora transmission
  LoRa.begin(LoRaFreq);

  // setup only for e-byte E-19 LoRa module
  pinMode(rxpin, OUTPUT);
  pinMode(txpin, OUTPUT);
  digitalWrite(rxpin, LOW);
  digitalWrite(txpin, HIGH);
  Serial.println("Data to Send:");

  for (int i = 0; i < transmitLen; i++)
  {
    Serial.print(transmitData[i]);
    Serial.print(", ");
  }
  Serial.println("");

  Serial.println("Begin Transmission");

  // send packet via LoRa
  LoRa.beginPacket();
  LoRa.write(transmitData, transmitLen);
  LoRa.endPacket();

  Serial.println("End of Transmission.");
  transmitData[0] = transmitData[0] + 1;
}