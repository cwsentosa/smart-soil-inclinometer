/*
================================================================================
|| Gateway Module Program
|| By Joshua Gunawan dan Charlie Tahar
================================================================================
*/

/*
================================================================================
|| Deklarasi Library, Objek, dan Global Variable
================================================================================
*/
#include <SoftwareSerial.h>

#define PCSerialBaud 9600
#define ATSerialBaud 9600
#define ATTimeout 1000
// #define APN_Address "Internet"

SoftwareSerial mySerial(A0, A1); // RX, TX

char response[200] = {};


/*
================================================================================
|| FUNCTION PROTOTYPE
================================================================================
*/

// Get ATCommand Reply From Serial
void ATGetReply(char msg[]);

// Get AT module status
bool ATCheck();

// Set APN to "Internet" (default)
bool ATSetAPN();

// Connect Module to Internet
bool ATConn();

// Initialize HTTP Service
bool ATHTTPInit();

// Terminate HTTP Service, Need to perform HTTPInit again
bool ATHTTPTerm();

// HTTP Service: GET Method
bool ATHTTPGET();

// HTTP Service: POST Method
bool ATHTTPPOST();


/*
================================================================================
|| MAIN ALGORITHM
================================================================================
*/
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(PCSerialBaud);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // Set the data rate for the SoftwareSerial port
  mySerial.begin(ATSerialBaud);
  mySerial.setTimeout(ATTimeout);

  delay(1000);
  
  ATCheck();
}

void loop() {
  
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}


/*
================================================================================
|| FUNCTION DEFINITION
================================================================================
*/

void ATGetReply(char msg[]){
  int i = 0;
  char b;
  do {
    b = mySerial.read();
    while (b != '\r' && b != '\n' ) {
      response[i] = b;
      i++;
      b = mySerial.read();
    }
  } while (b != '\n');
  response[i] = '\0';
}

bool ATCheck(){
  mySerial.write("AT\r\n");
  response = {};
  ATGetReply(response);

  // Print Reply for Debugging
  Serial.print("Response: ");
  Serial.println(response);
  Serial.println("End");

  // If Echo is ON
  if (strcmp(response, "AT\r\n")){
    // Turn Echo OFF and Save Config to Non Volatile Memory
    mySerial.write("ATE0&W\r\n");

    response = {};
    ATGetReply(response);
  }

  if (strcmp(response, "OK\r\n")){
    return true;
  }
  else
    return false;
}

bool ATSetAPN(){
  mySerial.write("AT+SAPBR=3,1,\"APN\",\"Internet\"\r\n");
  response = {};
  ATGetReply(response);

  if(strcmp(response, "OK\r\n")){
    return true;
  }
  else 
    return false;
}

bool ATConn(){
  mySerial.write("AT+SAPBR=1,1\r\n");
  response = {};
  ATGetReply(response);

  if(strcmp(response, "OK\r\n")){
    return true;
  }
  else 
    return false;
}

bool ATHTTPInit(){
  mySerial.write("ATHTTPINIT");
  response = {};
  ATGetReply(response);

  if(strcmp(reponse, "OK\r\n")){
    return true;
  }
  else
    return false;
}

bool ATHTTPTerm(){
  mySerial.write("ATHTTPTERM");
  response = {};
  ATGetReply(response);

  if(strcmp(reponse, "OK\r\n")){
    return true;
  }
  else
    return false;
}