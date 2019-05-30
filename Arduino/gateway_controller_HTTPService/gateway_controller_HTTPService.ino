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
#define PCSerialBaud 9600
#define ATSerialBaud 9600
#define ATTimeout 1000
#include <SoftwareSerial.h>
#include "ATcommand_HTTP.hpp"
SoftwareSerial mySerial(A0, A1); // RX, TX
/*
================================================================================
|| MAIN ALGORITHM
================================================================================
*/
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(PCSerialBaud);
  while (!Serial) {
    ;
  }

  // Set the data rate for the SoftwareSerial port
  mySerial.begin(ATSerialBaud);
  mySerial.setTimeout(ATTimeout);
  delay(1000);

  while(!initHTTPService());
  delay(500);
  
  char url_post[] = "http://www.gundala.co.id/ssi/ssi_post.php";
  char url_get[]    = "http://www.gundala.co.id/ssi/ssi_get.php";
  char data[] = "gid=1&nid=1&sid=1&date=2019-05-23&time=12\%3A00\%3A00&data0=0&data1=0&data2=0&data3=0&data4=0&data5=0&data6=0&data7=0&data8=0&data9=0&data10=0&data11=0&data12=0&data13=0&data14=0";
  sendData(url_post, data, 1);
  delay(500);
  closeHTTPService();
  
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }

}


