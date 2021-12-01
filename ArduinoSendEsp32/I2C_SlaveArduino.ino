#include <Wire.h>


// I2C slave Arduino
#define SLAVE_ADDR 9
#define DELAY_TIME 100

// Define sensors
#define TEMP 1
#define MOTOR 2
#define PH 3

int sensorNo = 0;


float getTemp();
float getRPM();
float getPH();

void setTemp();
void setRPM();
void setPH();


float readSensor() {
  float result = 0.0;
  switch (sensorNo) {
    case TEMP:
      result = getTemp();
      break;
    case MOTOR:
      result = getRPM();
      break;
    case PH:
      result = getPH();
      break;
    default:
      break;
  }

  return result;
}


void writeSensor() {
  switch (sensorNo) {
    case TEMP:
      setTemp();
      break;
    case MOTOR:
      setRPM();
      break;
    case PH:
      setPH();
      break;
    default:
      break;
  }
}


void setup() {
  Wire.begin(SLAVE_ADDR); // Join i2c bus with slave address
 
  // Function to run when data requested from master
  Wire.onRequest(requestEvent); 
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);
  
  Serial.begin(9600);
  Serial.println("I2C Slave Arduino");
}

void loop() {
  delay(DELAY_TIME);
}

void requestEvent() {
  Serial.println("Request event");

  float result = readSensor();
  String result_str = String(result, 3); // Convert float to string with 3 DP precision

  Wire.write(result_str); 
}

void receiveEvent() {
  Serial.println("Receive event");

  char received_string[10];
  int counter = 0;
  char *token;
  float fvalue;
    
  while (Wire.available()) {
    received_string[counter++] = Wire.read();
  }
  
  // 1st token is operation name in string
  char *operation = strtok(received_string, " ");

  // 2nd token is sensor number in int
  token = strtok(NULL, " ");
  if (token != NULL){
  int sensorNo = (token-'0') - 1;
  }
  
  // 3rd token is value in float
  char *strvalue = strtok(NULL, " ");
  if (strvalue != NULL) {
    // str -> float
    fvalue = atof(strvalue);
  }
  
  if (strcmp(operation, "SET") == 0){
    writeSensor();
  }
  
  else if (strcmp(operation, "STOP") == 0){
    // Raise infinite loop to stop running of code
    while (1);
  }
}

  
  
  
 
