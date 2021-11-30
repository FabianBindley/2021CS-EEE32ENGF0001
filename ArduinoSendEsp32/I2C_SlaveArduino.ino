// I2C slave Arduino
#define SLAVE_ADDR 9
#include <Wire.h>
float degreeC=35.0;
float RPM = 200.0;
float PH = 7.0;
float sensors[3] = {degreeC,RPM,PH};
int sensorNo = 0;


void setup() {
 Wire.begin(SLAVE_ADDR); // join i2c bus with address #8
 
  // Function to run when data requested from master
 Wire.onRequest(requestEvent); 
  // Function to run when data received from master
 Wire.onReceive(receiveEvent);
  
 Serial.begin(9600);
 Serial.println("I2C Slave Arduino");
}

void loop() {
 delay(100);
}

void requestEvent() {
 Wire.write((byte)sensors[sensorNo]); 
 Serial.println("Request event");
}

void receiveEvent() {
    char received_string[10];
    int counter = 0;
    char *token;
    float fvalue;
    
    while (Wire.available()) {
      received_string[counter++] = Wire.read();
    }
  
    // 1st token is operation name in string
   char* operation = strtok(received_string, " ");   
   // 2nd token is sensor number in int
   token = strtok(NULL, " ");
   if (token != NULL){
    int sensorNo = (token-'0') - 1;
   }
   // 3rd token is value in float
   char* strvalue = strtok(NULL, " ");
   if (strvalue != NULL){
    // str -> float
    fvalue = atof(strvalue);
   }
   
  
  
  if (strcmp(operation, "SET") == 0){
    sensors[sensorNo]=fvalue;
    
  }
  
  else if (strcmp(operation, "STOP") == 0){
    // operation is STOP, raises SysExit
  }
  
  Serial.println("Receive event");
  }

  
  
  
 
