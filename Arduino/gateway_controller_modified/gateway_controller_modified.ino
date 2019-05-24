#include "lora_function.hpp"

char buffer[STRING_BUFFER_LENGTH] = "";

void setup()
{
  Serial.begin(SERIAL_FREQ);
  while (!Serial)
    ;

  Serial.println("Gateway controller demo");

  setupLora();
}

/*
============================================================================================================================
|| MAIN FUNCTION
============================================================================================================================
*/

void loop()
{
  receiveAndParseData(buffer);
  int strcmp_val = strcmp(buffer, "No message");
  if (strcmp_val) {
    Serial.print("buffer: "); Serial.println(buffer);  
  }
  sprintf(buffer, "No message");

  delay(20);
}
