#include <SPI.h>
#include <LoRa.h>

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/

uint8_t clientId = 212;
uint8_t generalId = 215;
uint8_t checkSum = 200;
uint8_t sumData = 0;

uint8_t receiveData[6] = {0,0,0,0,0,0};
const uint8_t receiveLen = sizeof(receiveData);

uint8_t transmitData[10] = {0,0,0,0,0,0,0,0,0,0};
const uint8_t transmitLen = sizeof(transmitData);

uint8_t cmdCode;

/* Received packet data from gateway via LoRa wireless
|-----------|------------|------------|---------|--------|--------|
| Byte 0    | Byte 1     | Byte 2     | Byte 3  | Byte 4 | Byte 5 |
|-----------|------------|------------|---------|--------|--------|
| Sensor ID | Client ID  | Check Code | Command | Data0  | Data1  |
|-----------|------------|------------|---------|--------|--------|
*/

/* Packet data stucture to send to sensor via serial
|-----------|------------|----------|----------|----------|
| Coloum 0  | Coloum 1   | Coloum 2 | Coloum 3 | Coloum 4 |
|-----------|------------|----------|----------|----------|
| Sensor ID | Client ID  | Command  | Data0    | Data1    |
|-----------|------------|----------|----------|----------|
*/

/* Received packet data structure from sensor via serial
|-----------|------------|-------------|------------|-------------|------------|-------------|------------|
| Coloum 0  | Coloum 1   | Coloum 2    | Coloum 3   | Coloum 4    | Coloum 5   | Coloum 6    | Coloum 7   |
|-----------|------------|-------------|------------|-------------|------------|-------------|------------|
| Sensor ID | Client ID  | AccelX High | AccelX Low | AccelY High | AccelY Low | AccelZ High | AccelZ Low |
|-----------|------------|-------------|------------|-------------|------------|-------------|------------|
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
|| SERIAL SETUP
============================================================================================================================
*/

uint32_t serFreq = 38400;
uint32_t serTo = 100;

/*
============================================================================================================================
|| SETUP FUNCTION
============================================================================================================================
*/

void setup() {

  Serial.begin(serFreq);
  while (!Serial);

  Serial.println("Gateway hub common");
  
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin
  
  if (!LoRa.begin(LoRaFreq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

/*
============================================================================================================================
|| MAIN FUNCTION
============================================================================================================================
*/

void loop() {

  // check for incoming a LoRa wireless signal from gateway and store the data to receive data
  if(receiveFunction(receiveData, receiveLen)){

    cmdCode = receiveData[3];

    switch(cmdCode){
      case 0 :
        break;
      
      case 1 :
        // send receive data to sensor
        printSerFunction(receiveData, receiveLen);
        break;
               
      case 2 :
        // send receive data to sensor
        printSerFunction(receiveData, receiveLen);
        // receive data from sensor
        readSerFunction(transmitData, transmitLen, serTo);
        // send data from sensor to gateway
        transmitFunction
        break;

      default :
        break;
    }
    
  }
  
}

/*
============================================================================================================================
|| TRANSMIT FUNCTION
============================================================================================================================
*/

void transmitFunction(uint8_t data[], uint8_t dataLen){

  // setup only for e-byte E-19 LoRa module
  pinMode(rxpin, OUTPUT);
  pinMode(txpin, OUTPUT);
  digitalWrite(rxpin, LOW);
  digitalWrite(txpin, HIGH);

  // avoid to send stop command
  if(data[3] == 0){ return; }

  // send transmitData via LoRa
  LoRa.beginPacket();
  LoRa.write(data, dataLen);
  LoRa.endPacket();
  
}

/*
============================================================================================================================
|| RECEIVE FUNCTION
============================================================================================================================
*/

bool receiveFunction(uint8_t data[], uint8_t dataLen){

  // setup only for e-byte E-19 LoRa module
  pinMode(rxpin, OUTPUT);
  pinMode(txpin, OUTPUT);
  digitalWrite(rxpin, HIGH);
  digitalWrite(txpin, LOW);

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    // read packet from LoRa
    uint8_t idx = 0;
    while (LoRa.available()) {
      if(idx < dataLen){
        // store data per-byte to receiveData
        data[idx] = LoRa.read();
        idx++;
      }else{
        // clear the data in case any signal contain longer byte than receiveLen
        LoRa.read();
      }
    }

    return true;
  }
  
  return false;
}

/*
============================================================================================================================
|| SERIAL FUNCTION
============================================================================================================================
*/

void printSerFunction(uint8_t data[], uint8_t dataLen){

  char arrayChar[3];
  uint8_t bufLen = (dataLen - 1) * 4;
  char buf[bufLen];
  int8_t i, j, k;
  
  // print packet using serial communication to sensor
  // the packet structure is similar to CSV 
  
  k = 0;
  for(i=0; i<receiveLen; i++){
    // skip check code to send
    if(i != 2){
        byteToString(receiveData[i], arrayChar);
        for(j=0; j<3; j++){
          buf[k++] = arrayChar[j];
        }
        if(k < (bufLen - 1)){
          buf[k++] = ',';
        }
    }
  }
  buf[k] = '\n';
  
  Serial.write(buf, bufLen);
  
}

bool readSerFunction(uint8_t data[], uint8_t dataLen, uint32_t timeOut){

  char arrayChar[4];
  uint32_t timeStart = millis();
  int8_t i, j;
  int8_t k = 0;
  
  for(j=0; j<dataLen; j++){
    
    // empty the arrayChar
    for(i=0; i<4; i++){
      arrayChar[i] = '\0'; 
    }

    // get serial data and assign it to arrayChar
    i = 0;
    while(i<4){
      if((millis() - timeStart) < timeOut){
        if(Serial.available()){
          arrayChar[i] = Serial.read();
          // end loop if received comma or new line character
          if(arrayChar[i] == ',' || arrayChar[i] == '\n'){
            break;
          }
          i++;
        }
      }else{
        // return false if timeout reached
        return false;
      }
    }

    // convert arayChar to byte and assign to transmitData
    data[j] = stringToByte(arrayChar);
    
    // end loop if received new line character
    if(arrayChar[i] == '\n'){
      break;
    }
    
  }

  // return false if stop command received. stop command is 0 on byte 3 of transmit data
  if(data[3] ==  0){
    return false;
  }

  return true;
}

void byteToString(uint8_t number, char numChar[3]){

  uint8_t divisor = 100;
  int8_t digit;

  for(int8_t i=0; i<3; i++){
    // calculate the digit number
    digit = number / divisor;
    
    // calculate divisor and number for next loop
    number = number - (divisor * digit);
    divisor = divisor / 10;
    
    // convert digit to ASCII character code
    numChar[i] = digit + 48;
  }

}

uint8_t stringToByte(char numChar[3]){

  uint8_t number = 0;
  uint8_t base = 1;
  int8_t digit;

  for(int8_t i=2; i>=0; i--){
    
    // check if the character is number character
    if(numChar[i] >=48 && numChar[i] <= 57){
      
      // determine the digit number based on ASCII
      digit = numChar[i] - 48;

      // calculate divisor and number for next loop
      number = number + (base * digit);
      base = base * 10;
      
    }
  }

  return number;
}


