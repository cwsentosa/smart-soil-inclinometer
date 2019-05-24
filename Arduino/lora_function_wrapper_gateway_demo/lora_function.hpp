#ifndef LORA_FUNCTION_H
#define LORA_FUNCTION_H

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define SERIAL_FREQ 9600
#define STRING_BUFFER_LENGTH 500

// MSG STRUCTURE
#define MSG_ID_IDX 0
#define INSTRUCTION_CODE_IDX 1
#define GATE_ID_HB_IDX 2
#define GATE_ID_LB_IDX 3
#define NODE_ID_IDX 4
#define BATT_IDX 5
#define CHECKSUM_IDX 6
#define SID_1_IDX 7
#define ACC_X_1_HB_IDX 8
#define ACC_X_1_LB_IDX 9
#define ACC_Y_1_HB_IDX 10
#define ACC_Y_1_LB_IDX 11
#define ACC_Z_1_HB_IDX 12
#define ACC_Z_1_LB_IDX 13
#define SID_2_IDX 14
#define ACC_X_2_HB_IDX 15
#define ACC_X_2_LB_IDX 16
#define ACC_Y_2_HB_IDX 17
#define ACC_Y_2_LB_IDX 18
#define ACC_Z_2_HB_IDX 19
#define ACC_Z_2_LB_IDX 20
#define SID_3_IDX 21
#define ACC_X_3_HB_IDX 22
#define ACC_X_3_LB_IDX 23
#define ACC_Y_3_HB_IDX 24
#define ACC_Y_3_LB_IDX 25
#define ACC_Z_3_HB_IDX 26
#define ACC_Z_3_LB_IDX 27
#define SID_4_IDX 28
#define ACC_X_4_HB_IDX 29
#define ACC_X_4_LB_IDX 30
#define ACC_Y_4_HB_IDX 31
#define ACC_Y_4_LB_IDX 32
#define ACC_Z_4_HB_IDX 33
#define ACC_Z_4_LB_IDX 34
#define SID_5_IDX 35
#define ACC_X_5_HB_IDX 36
#define ACC_X_5_LB_IDX 37
#define ACC_Y_5_HB_IDX 38
#define ACC_Y_5_LB_IDX 39
#define ACC_Z_5_HB_IDX 40
#define ACC_Z_5_LB_IDX 41

void setupLora();
void receiveAndParseData(char buffer[]);

#endif
