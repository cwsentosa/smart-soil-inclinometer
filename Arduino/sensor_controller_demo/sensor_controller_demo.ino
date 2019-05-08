#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

/*
============================================================================================================================
|| TRANSMIT AND RECEIVE DATA STRUCTURE
============================================================================================================================
*/

uint8_t sensorId = 1;           // Node ID
uint8_t generalId = 255;        // ID for all nodes to comunicate with gateway
uint8_t clientId = 1;      // Selected client ID to communicate

uint8_t transmitData[8] = {0,0,0,0,0,0,0,0};      // Transmit data: Byte array data to send using LoRa wireless
const uint8_t transmitLen = sizeof(transmitData); // Transmit data length

uint8_t receiveData[5] = {0,0,0,0,0};             // Receive data: Byte array data incoming from LoRa wireless
const uint8_t receiveLen = sizeof(receiveData);   // Receive data length

uint8_t meaCur[6] = {0,0,0,0,0,0};                // measurement data from current period
const uint8_t meaLen = sizeof(meaCur);            // measure data length
uint8_t meaPrev[6] = {0,0,0,0,0,0};               // measurement data from previous period

uint8_t cmdCode;
uint16_t cmdData;

/* Packet data structure to send via serial
|-----------|------------|-------------|------------|-------------|------------|-------------|------------|
| Coloum 0  | Coloum 1   | Coloum 2    | Coloum 3   | Coloum 4    | Coloum 5   | Coloum 6    | Coloum 7   |
|-----------|------------|-------------|------------|-------------|------------|-------------|------------|
| Sensor ID | Client ID  | AccelX High | AccelX Low | AccelY High | AccelY Low | AccelZ High | AccelZ Low |
|-----------|------------|-------------|------------|-------------|------------|-------------|------------|
*/

/* Received packet data structure via serial
|-----------|------------|----------|----------|----------|
| Coloum 0  | Coloum 1   | Coloum 2 | Coloum 3 | Coloum 4 |
|-----------|------------|----------|----------|----------|
| Sensor ID | Client ID  | Command  | Data0    | Data1    |
|-----------|------------|----------|----------|----------|
*/

/*
============================================================================================================================
|| SENSORS AND SERIAL SETUP
============================================================================================================================
*/

uint32_t serFreq = 38400;
uint32_t serTo = 100;

MPU6050 inclino;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

uint8_t inclFS = 0;
uint8_t inclDLPF = 6;

int16_t ofstX = 0;
int16_t ofstY = 0;
int16_t ofstZ = 0;

/*
============================================================================================================================
|| SETUP FUNCTION
============================================================================================================================
*/

void setup() {
  
  // begin serial communication
  Serial.begin(serFreq);
  
  // initialize sensor
  Wire.begin();
  inclino.initialize();
  
  // setup MPU6050 sensor
  inclino.setFullScaleAccelRange(inclFS);
  inclino.setDLPFMode(inclDLPF);
  inclino.setXAccelOffset(ofstX);
  inclino.setYAccelOffset(ofstY);
  inclino.setZAccelOffset(ofstZ);
  
}

/*
============================================================================================================================
|| MAIN FUNCTION
============================================================================================================================
*/

void loop() {
  
  // check incoming command sent by server gateway
  if(readSerFunction(receiveData, receiveLen, serTo)){
    
    // get command code and command data
    cmdCode = getCommand(receiveData, sensorId, generalId, clientId);
    cmdData = receiveData[3] * 256 + receiveData[4];

    switch(cmdCode){
      case 0 :
        Serial.end();
        delay(100);
        Serial.begin(serFreq);
        break;
      
      case 1 :
        measureFunction(cmdData, meaCur, meaPrev);
        break;
               
      case 2 :
        transmitDataFunction(transmitData, transmitLen, sensorId, clientId, meaCur, meaLen);
        delay(10);
        printSerFunction(transmitData, transmitLen);
        break;

      default :
        break;
      
    }
    
  }

}

/*
============================================================================================================================
|| MEASURE FUNCTION
============================================================================================================================
*/

void measureFunction(uint16_t maxSample, uint8_t dataCur[6], uint8_t dataPrev[6])
{

  int16_t ax, ay, az;             // acceleration readings variable
  int32_t axSum, aySum, azSum;    // used in averaging acceleration readings
  uint16_t nSample;               // used in averaging acceleration readings
  double axAvg, ayAvg, azAvg;     // used in averaging acceleration readings
  
  // reset I2C connection if connection to sensor failed
  if(!inclino.testConnection()){
    Wire.end();
    Wire.begin();
  }
  
  // reinitialize sensor
  inclino.initialize();
  // setup MPU6050 sensor
  inclino.setFullScaleAccelRange(inclFS);
  inclino.setDLPFMode(inclDLPF);
  inclino.setXAccelOffset(ofstX);
  inclino.setYAccelOffset(ofstY);
  inclino.setZAccelOffset(ofstZ);
  
  // averaging sensor value
  nSample = 0;
  axSum = 0;
  aySum = 0;
  azSum = 0;
  while(nSample < maxSample){
    // get sensor value
    inclino.getAcceleration(&ax, &ay, &az);
    axSum += ax;
    aySum += ay;
    azSum += az;
    nSample++;
    delay(1);
  }
  axAvg = (double) axSum / nSample;
  ayAvg = (double) aySum / nSample;
  azAvg = (double) azSum / nSample;

  ax = round(axAvg);
  ay = round(ayAvg);
  az = round(azAvg);

  // store last measurement data
  dataPrev[0] = dataCur[0];
  dataPrev[1] = dataCur[1];
  dataPrev[2] = dataCur[2];
  dataPrev[3] = dataCur[3];
  dataPrev[4] = dataCur[4];
  dataPrev[5] = dataCur[5];

  // store current measurement data
  dataCur[0] = ax >> 8;
  dataCur[1] = ax;
  dataCur[2] = ay >> 8;
  dataCur[3] = ay;
  dataCur[4] = az >> 8;
  dataCur[5] = az;

//  Serial.print(ax);
//  Serial.print('\t');
//  Serial.print(ay);
//  Serial.print('\t');
//  Serial.println(az);

}


/*
============================================================================================================================
|| SERIAL FUNCTION
============================================================================================================================
*/

void printSerFunction(uint8_t data[], uint8_t dataLen){

  char arrayChar[3];
  uint8_t bufLen = dataLen * 4;
  char buf[bufLen];
  int8_t i, j, k;
  
  // print packet using serial communication to gateway (raspberry pi devtty S0)
  // the packet structure is similar to CSV 
  
  k = 0;
  for(i=0; i<dataLen; i++){
      byteToString(data[i], arrayChar);
      for(j=0; j<3; j++){
        buf[k++] = arrayChar[j];
      }
      if(i<(dataLen-1)){
        buf[k++] = ',';
      }else{
        buf[k++] = '\n';
      }
  }

  Serial.write(buf, bufLen);
  
}

void transmitDataFunction(uint8_t data[], uint8_t dataLen, uint8_t nodeid, uint8_t gatewayid, uint8_t dataIn[], uint8_t dataInLen){
  
  // asign id to transmit data
  data[0] = sensorId;
  data[1] = clientId;
  
  // asign measurement data to transmit data
  for(uint8_t i=0; i<dataInLen; i++){
    data[i+dataLen-dataInLen] = dataIn[i];
  }
  
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

  return true;
}

uint8_t getCommand(uint8_t data[], uint8_t nodeid, uint8_t generalid, uint8_t gatewayid){
  
  // checking the node ID
  if(data[0] != nodeid && data[0] != generalid){
    return 0;
  }
  
  // checking the access code
  if(data[1] != gatewayid){
    return 0;
  }

  // return the command code
  return data[2];
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


