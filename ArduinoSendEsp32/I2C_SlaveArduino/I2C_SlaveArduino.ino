#include <Wire.h>


// I2C slave Arduino
#define SLAVE_ADDR 9
#define DELAY_TIME 100
#define PRECISION 4

// Define sensors
#define TEMP 1
#define MOTOR 2
#define PH 3

#define SERIAL_DEBUG_BAUD 115200


// Define constants
const float A = 0.0012323996168659172;
const float B = 0.00022347497515516762;
const float C = 8.354794023823214e-08;
const float kelvinOffset = 273.15;
const float voltageInput = 5;
const int range = 1024;
const int resistorValue = 10000;

// Define variables
int sensorNo = 0;

int vInput = 0;
int heaterOutputPin = 3;
int thermistorInputPin = A0;
int heaterLEDpin = 4;

float kelvinTemp;
float celciusTemp;
float thermistorResistance;
float voltage;

float setpointTemperature = 30.0;
float temperatureLagOffset = 1.0;

float setpointRPM = 2400;
float setpointPH = 7.0;


void setTemp(float value) {
  setpointTemperature = value;
  Serial.print("Set temp to: ");
  Serial.println(setpointTemperature);
}


float getTemp() {
  vInput = analogRead(thermistorInputPin);
  // delay(100); what's this for
  voltage = vInput / (range / voltageInput);

  Serial.print("Voltage: ");
  Serial.println(voltage);
  
  //We have voltage now we need resistance
  thermistorResistance = voltage * resistorValue / (voltageInput - voltage);
  
  kelvinTemp = 1 / (A + B * log(thermistorResistance) + C * (log(thermistorResistance) * log(thermistorResistance) * log(thermistorResistance)));
  celciusTemp = kelvinTemp - kelvinOffset;

  return celciusTemp;
}


void setRPM(float value) {
  setpointRPM = value;
  Serial.print("Set RPM to: ");
  Serial.println(setpointRPM);
}


float getRPM();


void setPH(float value) {
  setpointPH = value;
  Serial.print("Set pH to: ");
  Serial.println(setpointPH);
}


float getPH();


void tempControl() {
  celciusTemp = getTemp();

  Serial.print("Celcius Temp: ");
  Serial.println(celciusTemp);
  
  //If the temperature is less than the required temp - the lag offset, turn on the heating circuit
  if (celciusTemp < (setpointTemperature - temperatureLagOffset)) {
    //Turn on the heater at half strength using 50% PWM
    analogWrite(heaterOutputPin, 127); 
    digitalWrite(heaterLEDpin, HIGH);
    Serial.println("Heater On");
  } else {
    //Turn off the heater
    Serial.println("Heater off");
    digitalWrite(heaterLEDpin, LOW);
    digitalWrite(heaterOutputPin, LOW);
  }
}


float readSensor() {
  float result = 0.0;
  if (sensorNo == TEMP) {
    result = getTemp();
  } else if (sensorNo == MOTOR) {
    result = getRPM();
  } else if (sensorNo == PH) {
    result = getPH();
  }

  return result;
}


void writeSensor(float value) {
  if (sensorNo == TEMP) {
    setTemp();
  } else if (sensorNo == MOTOR) {
    setRPM();
  } else if (sensorNo == PH) {
    setPH();
  }
}


void setup() {
  Wire.begin(SLAVE_ADDR); // Join i2c bus with slave address
 
  // Function to run when data requested from master
  Wire.onRequest(requestEvent); 
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);

  // Heater/thermistor pins
  pinMode(heaterOutputPin,OUTPUT);
  pinMode(heaterLEDpin,OUTPUT);
  pinMode(thermistorInputPin,INPUT);
  
  // Begin debug Serial Monitor
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println("I2C Slave Arduino");
}

void loop() {
  delay(DELAY_TIME);

  // Handle control of sensors
  tempControl();
}

void requestEvent() {
  Serial.println("Request event");

  float result = readSensor();

  char result_str[10];
  dtostrf(result, 4, 4, result_str);  //4 is mininum width, 4 is precision; float value is copied onto buff

  Wire.write(result_str); 
}

void receiveEvent(int num_bytes) {
  Serial.println("Receive event");

  char received_string[num_bytes];
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
  if (token != NULL) {
    int sensorNo = atoi(token);
  }
  
  // 3rd token is value in float
  char *strvalue = strtok(NULL, " ");
  if (strvalue != NULL) {
    // str -> float
    fvalue = atof(strvalue);
  }
  
  if (strcmp(operation, "SET") == 0){
    writeSensor(fvalue);
  }
  
  else if (strcmp(operation, "STOP") == 0){
    // Raise infinite loop to stop running of code
    while (1);
  }
}
