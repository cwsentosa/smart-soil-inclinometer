#include "lora_function.hpp"

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
|| SERIAL SETUP
============================================================================================================================
*/

uint8_t receiveData[42] = {0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0};
const uint8_t receiveLen = sizeof(receiveData);

uint8_t transmitData[6] = {0, 0, 0, 0, 0, 0};
const uint8_t transmitLen = sizeof(transmitData);

// Function Prototype
static void parseData(uint8_t data[], uint8_t dataLen, char buffer[]);
static bool receiveFunction(uint8_t data[], uint8_t dataLen);

void setupLora()
{
    // override the default CS, reset, and IRQ pins (optional)
    LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

    if (!LoRa.begin(LoRaFreq))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
}

void receiveAndParseData(char buffer[])
{
    // check for incoming a LoRa wireless signal from nodes and store the data to receive data
    if (receiveFunction(receiveData, receiveLen))
    {
        parseData(receiveData, receiveLen, buffer);
    }
}

/*
============================================================================================================================
|| RECEIVE FUNCTION
============================================================================================================================
*/

static void parseData(uint8_t data[], uint8_t dataLen, char buffer[])
{
    if (dataLen != receiveLen) {
      Serial.println("Data Length Error!");
      return false;
    } else {
      // char buffer[300] = "";
      //    uint8_t i, j;
      uint8_t msg_id;
      uint8_t instruction_code;
      uint16_t gid;
      uint8_t nid, batt_vol, checksum;
      uint8_t sensorID[5];
      int16_t sensorData[5][3];

      msg_id = data[MSG_ID_IDX];
      instruction_code = data[INSTRUCTION_CODE_IDX];
      gid = (data[GATE_ID_HB_IDX] << 8) + data[GATE_ID_LB_IDX];
      nid = data[NODE_ID_IDX];
      batt_vol = data[BATT_IDX];
      checksum = data[CHECKSUM_IDX];
  
      sensorID[0] = data[SID_1_IDX];
      sensorData[0][0] = (data[ACC_X_1_HB_IDX] << 8) + data[ACC_X_1_LB_IDX];
      sensorData[0][1] = (data[ACC_Y_1_HB_IDX] << 8) + data[ACC_Y_1_LB_IDX];
      sensorData[0][2] = (data[ACC_Z_1_HB_IDX] << 8) + data[ACC_Z_1_LB_IDX];

      sensorID[1] = data[SID_2_IDX];
      sensorData[1][0] = (data[ACC_X_2_HB_IDX] << 8) + data[ACC_X_2_LB_IDX];
      sensorData[1][1] = (data[ACC_Y_2_HB_IDX] << 8) + data[ACC_Y_2_LB_IDX];
      sensorData[1][2] = (data[ACC_Z_2_HB_IDX] << 8) + data[ACC_Z_2_LB_IDX];

      sensorID[2] = data[SID_3_IDX];
      sensorData[2][0] = (data[ACC_X_3_HB_IDX] << 8) + data[ACC_X_3_LB_IDX];
      sensorData[2][1] = (data[ACC_Y_3_HB_IDX] << 8) + data[ACC_Y_3_LB_IDX];
      sensorData[2][2] = (data[ACC_Z_3_HB_IDX] << 8) + data[ACC_Z_3_LB_IDX];

      sensorID[3] = data[SID_3_IDX];
      sensorData[3][0] = (data[ACC_X_4_HB_IDX] << 8) + data[ACC_X_4_LB_IDX];
      sensorData[3][1] = (data[ACC_Y_4_HB_IDX] << 8) + data[ACC_Y_4_LB_IDX];
      sensorData[3][2] = (data[ACC_Z_4_HB_IDX] << 8) + data[ACC_Z_4_LB_IDX];
      
      sensorID[4] = data[SID_3_IDX];
      sensorData[4][0] = (data[ACC_X_5_HB_IDX] << 8) + data[ACC_X_5_LB_IDX];
      sensorData[4][1] = (data[ACC_Y_5_HB_IDX] << 8) + data[ACC_Y_5_LB_IDX];
      sensorData[4][2] = (data[ACC_Z_5_HB_IDX] << 8) + data[ACC_Z_5_LB_IDX];
  
      sprintf(buffer, "msg_id: %u, instruction code: %u |gid=%u&nid=%u&batt=%u&checksum=%u&date=2019-05-09&time=10\%3A00\%3A03&sid0=%d&data00=%d&data01=%d&data02=%d&sid1=%d&data10=%d&data11=%d&data12=%d&sid2=%d&data20=%d&data21=%d&data22=%d&sid3=%d&data30=%d&data31=%d&data32=%d&sid4=%d&data40=%d&data41=%d&data42=%d",
              msg_id, instruction_code,
              gid, nid, batt_vol, checksum,
              sensorID[0], sensorData[0][0], sensorData[0][1], sensorData[0][2],
              sensorID[1], sensorData[1][0], sensorData[1][1], sensorData[1][2],
              sensorID[2], sensorData[2][0], sensorData[2][1], sensorData[2][2],
              sensorID[3], sensorData[3][0], sensorData[3][1], sensorData[3][2],
              sensorID[4], sensorData[4][0], sensorData[4][1], sensorData[4][2]);  
    }
}

static bool receiveFunction(uint8_t data[], uint8_t dataLen)
{
    // setup only for e-byte E-19 LoRa module
    pinMode(rxpin, OUTPUT);
    pinMode(txpin, OUTPUT);
    digitalWrite(rxpin, HIGH);
    digitalWrite(txpin, LOW);

    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {

        // read packet from LoRa
        uint8_t idx = 0;
        while (LoRa.available())
        {
            if (idx < dataLen)
            {
                // store data per-byte to receiveData
                data[idx] = LoRa.read();
                idx++;
            }
            else
            {
                // clear the data in case any signal contain longer byte than receiveLen
                LoRa.read();
            }
        }

        return true;
    }

    return false;
}
