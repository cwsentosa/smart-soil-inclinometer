#include <SPI.h>
#include <LoRa.h>

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/

// uint8_t gatewayId = 214;
// uint8_t generalId = 0;
// uint8_t checkSum = 200;
// uint8_t sumData = 0;

uint8_t receiveData[40] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
const uint8_t receiveLen = sizeof(receiveData);

uint8_t transmitData[6] = {0,0,0,0,0,0};
const uint8_t transmitLen = sizeof(transmitData);

/* Received packet data from node via LoRa wireless
|----------|------------|------------|-------------|--------|--------|--------|--------|--------|--------|
| Byte 0   | Byte 1     | Byte 2     | Byte 3      | Byte 4 | Byte 5 | Byte 6 | Byte 7 | Byte 8 | Byte 9 |
|----------|------------|------------|-------------|--------|--------|--------|--------|--------|--------|
| Node ID  | Gateway ID | Check Code | Bat Voltage | Data0  | Data1  | Data2  | Data3  | Data4  | Data5  |
|----------|------------|------------|-------------|--------|--------|--------|--------|--------|--------|
*/

/* Packet data stucture to send via serial
|----------|------------|------------|-------------|----------|----------|----------|----------|----------|----------|-----------|-----------|
| column 0 | column 1   | column 2   | column 3    | column 4 | column 5 | column 6 | column 7 | column 8 | column 9 | column 10 | column 11 |
|----------|------------|------------|-------------|----------|----------|----------|----------|----------|----------|-----------|-----------|
| Node ID  | Gateway ID | Check Code | Bat Voltage | Data 0   | Data 1   | Data 2   | Data 3   | Data 4   | Data 5   | RSSI      | SNR       |
|----------|------------|------------|-------------|----------|----------|----------|----------|----------|----------|-----------|-----------|
*/

/* Received packet data structure via serial
|----------|------------|------------|--------------|----------|----------|
| column 0 | column 1   | column 2   | column 3     | column 4 | column 5 |
|----------|------------|------------|--------------|----------|----------|
| Node ID  | Gateway ID | Check Code | Command Type | Data 0   | Data 1   |
|----------|------------|------------|--------------|----------|----------|
*/

/* Packet data structure to send to node via LoRa wireless
|----------|------------|------------|--------------|--------|--------|
| Byte 0   | Byte 1     | Byte 2     | Byte 3       | Byte 4 | Byte 5 |
|----------|------------|------------|--------------|--------|--------|
| Node ID  | Gateway ID | Check Code | Command Type | Data 1 | Data 2 |
|----------|------------|------------|--------------|--------|--------|
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

uint32_t LoRaFreq = 915E6;        // LoRa wireless communication frequency

/*
============================================================================================================================
|| SERIAL SETUP
============================================================================================================================
*/

uint32_t serFreq = 9600;

/*
============================================================================================================================
|| SETUP FUNCTION
============================================================================================================================
*/

bool receiveFunction(uint8_t data[], uint8_t dataLen);
void parseData(uint8_t data[], uint8_t result[]);

void setup() {
  Serial.begin(serFreq);
  while (!Serial);

  Serial.println("Gateway controller demo");
  
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

  // check for incoming a LoRa wireless signal from nodes and store the data to receive data
  if (receiveFunction(receiveData, receiveLen)){
    parseData(receiveData, receiveLen);  
  }

}

/*
============================================================================================================================
|| RECEIVE FUNCTION
============================================================================================================================
*/

void parseData(uint8_t data[], uint8_t dataLen){

  char buffer[300] = "";

  uint8_t i, j;
  uint16_t gid;
  uint8_t nid, batt_vol, checksum;
  uint8_t sensorID[5];
  int16_t sensorData[5][3];

  gid       = (data[0] << 8) + data[1];
  nid       = data[2];
  batt_vol  = data[3];
  checksum  = data[4];

  sensorID[0]       = data[5];
  sensorData[0][0]  = (data[ 6] << 8) + data[ 7];
  sensorData[0][1]  = (data[ 8] << 8) + data[ 9];
  sensorData[0][2]  = (data[10] << 8) + data[11];

  sensorID[1]       = data[12];
  sensorData[1][0]  = (data[13] << 8) + data[14];
  sensorData[1][1]  = (data[15] << 8) + data[16];
  sensorData[1][2]  = (data[17] << 8) + data[18];

  sensorID[2]       = data[19];
  sensorData[2][0]  = (data[20] << 8) + data[21];
  sensorData[2][1]  = (data[22] << 8) + data[23];
  sensorData[2][2]  = (data[24] << 8) + data[25];

  sensorID[3]       = data[26];
  sensorData[3][0]  = (data[27] << 8) + data[28];
  sensorData[3][1]  = (data[29] << 8) + data[30];
  sensorData[3][2]  = (data[31] << 8) + data[32];

  sensorID[4]       = data[33];
  sensorData[4][0]  = (data[34] << 8) + data[35];
  sensorData[4][1]  = (data[36] << 8) + data[37];
  sensorData[4][2]  = (data[38] << 8) + data[39];

  // strcat = (buffer, "gid=");
  // strcat = (buffer, itoa(gid));
  // strcat = (buffer, "&nid=");
  // strcat = (buffer, itoa(nid));
  // strcat = (buffer, "&batt=");
  // strcat = (buffer, itoa(batt_vol));

  sprintf(buffer, "gid=%u&nid=%u&batt=%u&checksum=%u&date=2019-05-09&time=10%%3A00%%3A03&sid0=%d&data00=%d&data01=&data02=%d&sid1=%d&data10=%d&data11=&data12=%d&sid2=%d&data20=%d&data21=&data22=%d&sid3=%d&data30=%d&data31=&data32=%d&sid4=%d&data40=%d&data41=&data42=%d", 
          gid, nid, batt_vol, checksum, 
          sensorID[0], sensorData[0][0],sensorData[0][1],sensorData[0][2],
          sensorID[1], sensorData[1][0],sensorData[1][1],sensorData[1][2],
          sensorID[2], sensorData[2][0],sensorData[2][1],sensorData[2][2],
          sensorID[3], sensorData[3][0],sensorData[3][1],sensorData[3][2],
          sensorID[4], sensorData[4][0],sensorData[4][1],sensorData[4][2]);

  Serial.println(buffer);

/*  j = 0;
  for (i = 0; i < 35; i++){ 
    if(i % 3 == 2){
      sensorID[j] = data[i];
    }
    else if (i % 3 == 0){}
  }
*/  
}
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
        // Serial.print("Data ");
        // Serial.print(idx+1);
        // Serial.print(":");
        // Serial.println(data[idx]);
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
  uint8_t bufLen = 160;
  char buf[bufLen];
  int8_t i, j, k;
  
  // print packet using serial communication to gateway (raspberry pi devtty S0)
  // the packet structure is similar to CSV 
  
  k = 0;
  for(i=0; i<receiveLen; i++){
      byteToString(receiveData[i], arrayChar);
      for(j=0; j<3; j++){
        buf[k++] = arrayChar[j];
      }
      buf[k++] = ',';
  }
  Serial.println(buf);
  
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
