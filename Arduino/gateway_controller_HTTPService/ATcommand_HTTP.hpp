
/*
================================================================================
|| AT COMMAND HEADER FILE
|| By Joshua Gunawan dan Charlie Tahar
================================================================================
*/

#ifndef ATCOMMAND_HTTP_HPP
#define ATCOMMAND_HTTP_HPP
#endif

#include <Arduino.h>
#include <SoftwareSerial.h>

#ifndef SERIAL_INCLUDE
extern HardwareSerial Serial;
extern SoftwareSerial mySerial;
#define SERIAL_INCLUDE
#endif



/*
================================================================================
|| FUNCTION PROTOTYPE
================================================================================
*/

// Initialize HTTP Service: Set APN and Bearer
bool initHTTPService();

// Send Data with GET(0) or POST(1) Method
bool sendData(char url[], char data[], int method);

// Close HTTP Service: Close Bearer;
bool closeHTTPService();
