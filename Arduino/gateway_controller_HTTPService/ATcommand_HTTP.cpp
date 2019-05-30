/*
================================================================================
|| AT COMMAND SOURCE FILE
|| By Joshua Gunawan dan Charlie Tahar
================================================================================
*/
#include "ATcommand_HTTP.hpp"

/*
================================================================================
|| FUNCTION PROTOTYPE
================================================================================
*/

// Get ATCommand Reply From Serial
void ATGetReply(char msg[], int msg_len);

// Get AT module status
bool ATCheck();

// Set APN to "Internet" (default)
bool ATSetAPN();

// Connect Module to Internet
bool ATConn();

// Disconnect Module to Internet
bool ATDisconn();

// Initialize HTTP Service
bool ATHTTPInit();

// Terminate HTTP Service, Need to perform HTTPInit again
bool ATHTTPTerm();

// Terminate HTTP Service, Need to perform HTTPInit again
bool ATHTTPRead(char psResponse[200]);

// HTTP Service: GET Method
bool ATHTTPGet(char url[]);

// HTTP Service: POST Method
bool ATHTTPPost(char url[], char HTTPData[]);


/*
================================================================================
|| FUNCTION DEFINITION
================================================================================
*/

void ATGetReply(char msg[], int msg_len){
  int i = 0;
  char read_char;

  // Get First /r/n Response From Module
  do {
  	while(!mySerial.available());
    read_char = mySerial.read();
  } while (read_char != '\n');

  while(!mySerial.available());
  read_char = mySerial.read();

  // Get Response Message
  do {
      msg[i] = read_char;
      i++;
      while(!mySerial.available());
      read_char = mySerial.read();
  } while (!((msg[i-1] == '\r') && (read_char == '\n')) && (i < msg_len));
  msg[i-1] = '\0';
}

bool ATCheck(){

	char response[50] = {};

	Serial.println("AT Check");
  mySerial.write("AT\r\n");
  ATGetReply(response, 50);

  // Print Reply for Debugging
//  Serial.print("Response: ");
//  Serial.println(response);

  if (!strncmp(response, "OK", 2)){
    return true;
  }
  else
    return false;
}

bool ATSetAPN(){

	char response[100] = {};

  mySerial.write("AT+SAPBR=3,1,\"APN\",\"Internet\"\r\n");
  ATGetReply(response, 100);

  if(!strncmp(response, "OK", 2)){
    return true;
  }
  else{
    return false;
  }
}

bool ATConn(){
  char response[100] = {};
  
  mySerial.write("AT+SAPBR=1,1\r\n");
  ATGetReply(response, 100);
  
  if(!strncmp(response, "OK", 2)){
    return true;
  }
  else{
    return false;
  }
  
}

bool ATDisconn(){
  char response[100] = {};
   
  mySerial.write("AT+SAPBR=0,1\r\n");
  ATGetReply(response, 100);
  
  if(!strncmp(response, "OK", 2)){
    return true;
  }
  else{
    return false;
  }
}

bool ATHTTPInit(){

	char response[100] = {};

  mySerial.write("AT+HTTPINIT\r\n");
  ATGetReply(response, 100);
  
  if(!strncmp(response, "OK", 2)){
    return true;
  }
  else{
    return false;
  }
}

bool ATHTTPTerm(){

	char response[100] = {};

  mySerial.write("AT+HTTPTERM\r\n");
  ATGetReply(response, 100);

  if(!strncmp(response, "OK", 2)){
    return true;
  }
  else{
    return false;
  }
}

// Terminate HTTP Service, Need to perform HTTPInit again
bool ATHTTPRead(char psResponse[200]){

  char response[50] = "";
  int readSize = 0;
  int i = 0;

  // Send Command to Read HTTP Data
	mySerial.write("AT+HTTPREAD\r\n");

	// Get Data Size
	ATGetReply(response, 50);
  sscanf(response, "+HTTPREAD: %d\r\n", &readSize);

  // Get HTTPREAD Result
  sprintf(psResponse, "");
  for (i = 0; i < readSize; i++){
    while(!mySerial.available());
    psResponse[i] = mySerial.read();
  }
  
  ATGetReply(response, 50);  
  if (!strncmp(response, "OK", 2)){
    return true;
  }else{
    return false;
  }
}

// HTTP Service: GET Method
bool ATHTTPGet(char url[]){

	char response[100] = "";
	char param[250] = "";
  int statusCode = 0;

	// Create Parameter to Change URL
	sprintf(param, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);

	// Send Command to Change URL
	mySerial.write(param);
	ATGetReply(response, sizeof(response));
  Serial.print("\nSet URL: ");
	Serial.println(response);

	if (!strncmp(response, "OK", 2)){
		// Execute GET Method
    Serial.println("Execute GET Method");
		mySerial.write("AT+HTTPACTION=0\r\n");

    // Get "OK" Reply
    sprintf(response, "");
    ATGetReply(response, sizeof(response));

    if (!strncmp(response, "OK", 2)){
      // Get "+HTTPACTION" Reply
      sprintf(response, "");
      ATGetReply(response, sizeof(response));
      
      sscanf(response, "+HTTPACTION: 0,%d", &statusCode);
      Serial.print("Status code: ");
      Serial.println(statusCode);
      
      if (statusCode == 200){
        return true;
      }
		}
	}
	return false;
}

// HTTP Service: POST Method
bool ATHTTPPost(char url[], char HTTPData[]){
	
	char response[100] = "";
	char param[150] = "";
  int statusCode = 0;
 
	unsigned int dataSize = strlen(HTTPData) + 10;

	// Set Content Type to Application Form
	mySerial.write("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n");
	ATGetReply(response, sizeof(response));
  Serial.println();
  Serial.print("Set Content Type: ");
	Serial.println(response);

	// Create Parameter to Change URL
	sprintf(param, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);

	// Send Command to Change URL
	mySerial.write(param);
	ATGetReply(response, sizeof(response));
  Serial.print("Set URL: ");
	Serial.println(response);

	// Set Data to Send
	sprintf(param, "");
  sprintf(param, "AT+HTTPDATA=%d,1000\r\n", dataSize);
	mySerial.write(param);
	ATGetReply(response, sizeof(response));

  if(!strncmp(response, "DOWNLOAD", 8)){
    mySerial.write(HTTPData);
    
    sprintf(response, "");
    ATGetReply(response, sizeof(response));
    Serial.print("Set Data: ");
    Serial.println(response);
  }
	
	if(!strncmp(response, "OK", 2)){
		// Execute POST Method
    Serial.println("Execute POST Method");
		mySerial.write("AT+HTTPACTION=1\r\n");

    // Get "OK" Reply
		sprintf(response, "");
		ATGetReply(response, sizeof(response));

    if (!strncmp(response, "OK", 2)){
      // Get "+HTTPACTION" Reply
      sprintf(response, "");
      ATGetReply(response, sizeof(response));
      
      sscanf(response, "+HTTPACTION: 1,%d", &statusCode);
      Serial.print("Status code: ");
      Serial.println(statusCode);
      
  		if (statusCode == 200){
  			return true;
  		}
    }
	}
	return false;
}

// Initialize HTTP Service: Set APN and Bearer
bool initHTTPService(){
  if(ATCheck()){
    if (ATSetAPN() && ATConn()){
      Serial.println("Initialize success");
      return true;
    }
    else{
      ATDisconn();
      Serial.println("Initialize failed");
      return false;
    }
  }else{
    Serial.println("Module not ready");
    return false;
  }
}

// Send Data with GET(0) or POST(1) Method
bool sendData(char url[], char data[], int method){
  char balasan[200] = "";
  int url_size = strlen(url) + strlen(data) + 3;
  char url_temp[url_size] = "";
  /**/
  if (method == 0){
    // GET METHOD
    ATHTTPInit();
    Serial.println("Get Method Selected");
    sprintf(url_temp,"%s?%s",url,data);
    
    ATHTTPGet(url_temp);

    ATHTTPRead(balasan);
    Serial.print("Balasan: ");
    Serial.println(balasan);
    ATHTTPTerm();
    
  }else if (method == 1){
    // POST METHOD
    ATHTTPInit();
    Serial.println("Post Method Selected");
    ATHTTPPost(url, data);

    ATHTTPRead(balasan);
    Serial.print("Balasan: ");
    Serial.println(balasan);
    ATHTTPTerm();
    
  }else {
    // UNKNOWN METHOD
    Serial.println("Unkown Method");
  }
  /**/
}

// Close HTTP Service: Close Bearer;
bool closeHTTPService(){
  if(ATDisconn()){
    Serial.println("HTTP Service Closed");
    return true;
  }else{
    return false;
  }
}

