#include <SPI.h>
#include <LoRa.h>
#include "LowPower.h"
#include <math.h>

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/

uint8_t clientId = 10;        // Client ID
uint8_t gatewayId = 214;      // Selected gateway ID to communicate
uint8_t generalId = 255;        // Node ID for gateway to access every node
uint8_t checkSum = 200;       // Check sum code, must be same with corresponding gateway and its sensor unit group
uint8_t sumData = 0;          // Variable used in generating checksum

uint8_t transmitData[6] = {10,100,0,1,0,0};    // Transmit data: Byte array data to send using LoRa wireless
const uint8_t transmitLen = sizeof(transmitData);    // Transmit data length

uint8_t receiveData[10] = {0,0,0,0,0,0,0,0,0,0};
const uint8_t receiveLen = sizeof(receiveData);

/* Packet data structure to send to client
|-----------|------------|------------|---------|--------|--------|
| Byte 0    | Byte 1     | Byte 2     | Byte 3  | Byte 4 | Byte 5 |
|-----------|------------|------------|---------|--------|--------|
| Sensor ID | Client ID  | Check Code | Command | Data0  | Data1  |
|-----------|------------|------------|---------|--------|--------|
*/

/* Packet data structure to send to gateway via LoRa wireless
|-----------|------------|-------------|-------------|-------------|------------|-------------|------------|-------------|------------|
| Byte 0    | Byte 1     | Byte 2      | Byte 3      | Byte 4      | Byte 5     | Byte 6      | Byte 7     | Byte 8      | Byte 9     |
|-----------|------------|-------------|-------------|-------------|------------|-------------|------------|-------------|------------|
| Sensor ID | Client ID  | Check Code  | Bat Voltage | AccelX High | AccelX Low | AccelY High | AccelY Low | AccelZ High | AccelZ Low |
|-----------|------------|-------------|-------------|-------------|------------|-------------|------------|-------------|------------|
*/

/*
============================================================================================================================
|| TIME AND PERIOD SETUP
============================================================================================================================
*/

// Period time: Data collecting cycle period of all nodes [milisecond]
// Must be same in every node program
uint32_t tPeriod = 5000;

// Start time of cycle [milisecond]
// Geeting value from millis()
uint32_t tStart;

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
|| SETUP FUNCTION
============================================================================================================================
*/

void setup()
{

  // set to low all unused pin of ATmega328P
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  // set to input IO pin which connected to reset pin
  pinMode(4, INPUT);

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
  
  // transmit the command data
  checksumCount();
  transmitFunction();

  // wait until next cycle
  tStart = tStart + tPeriod;
  while((millis() < tStart)){
    delay(1);
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

void checksumCount(){

  // calculate checksum code
  transmitData[2] = 0;
  sumData = 0;
  for(uint8_t i=0; i<transmitLen ;i++){
    sumData = sumData + transmitData[i];
  }
  // asign checksum code to transmit data
  transmitData[2] = checkSum - sumData;
}

/*
============================================================================================================================
|| RECEIVE FUNCTION
============================================================================================================================
*/

bool receiveFunction(uint16_t timeout){
/*
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  //Begin Lora transmission
  LoRa.begin(LoRaFreq);
*/  
  // setup only for e-byte E-19 LoRa module
  pinMode(rxpin, OUTPUT);
  pinMode(txpin, OUTPUT);
  digitalWrite(rxpin, HIGH);
  digitalWrite(txpin, LOW);

  // reset the receive data
  for(int i=0; i<receiveLen; i++){
      receiveData[i] = 0;
  }

  while(millis() < timeout){
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize == receiveLen) {
      
      // read packet from LoRa
      uint8_t idx = 0;
      while (LoRa.available()) {
        if(idx < receiveLen){
          receiveData[idx] = LoRa.read();
          idx++;
        }
      }

      // calculate the checksum of received data packet
      uint8_t sumData = 0;
      for(uint8_t i=0; i<receiveLen; i++){
        sumData = sumData + receiveData[i];
      }
      
      // checking the data integrity using checksum
      if(sumData != checkSum){
        return false;
      }
      
      // checking the node ID
      if(receiveData[0] != clientId){
        return false;
      }
      
      // checking the access code
      if(receiveData[1] != gatewayId){
        return false;
      }

      break;
    }
    
  }

  return true;
}

