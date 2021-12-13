#include <Wire.h>
#include "QuickMedianLib.h"


// I2C slave Arduino
#define SLAVE_ADDR 9
#define DELAY_TIME 100
#define PRECISION 4

// Define sensors
#define TEMP 1
#define MOTOR 2
#define PH 3

#define SERIAL_DEBUG_BAUD 115200


// Global variables
int sensorNo = 0;

int vInput = 0;
float voltage;


// Define constants
const float voltageInput = 5;


// Temperature
const float A = 0.0012323996168659172;
const float B = 0.00022347497515516762;
const float C = 8.354794023823214e-08;
const float kelvinOffset = 273.15;
const int range = 1024;
const int resistorValue = 10000;

int heaterOutputPin = 3;
int thermistorInputPin = A0;
int heaterLEDpin = 4;
int heaterOutputPower;

float kelvinTemp;
float celciusTemp;
float thermistorResistance;
float tempDifference;


// Motor
int targetspeed = 600;
float v;
float frequency;
int squareWavePin = 2;
int motorpin = 6;
int motorspeed = 60;  // note buzzing (this keeps changing?)
unsigned long long time1;
unsigned long long time2;
volatile int counter = 0;
volatile int loopCount = 0;
volatile int oscillated = 0;
float rpms[5];


// pH
bool acid;
bool alkaline;

int phvalue;
int pump1=11; //acid pump
int pump2=10; //alki pump

int x=0;
int y=0;
float Z = -16.65;
float Y = 48.402;
float voltage2;
float pH;
float probeRead;


// Setpoints
float setpointTemperature = 37.0;
float setpointRPM = 600;
float setpointPH = 6.6;


void setTemp(float value) {
  setpointTemperature = value;
  Serial.print("Set temp to: ");
  Serial.println(setpointTemperature);
}


float getInstantTemp() {
  vInput = analogRead(thermistorInputPin);
  voltage = 5 - (vInput / (range / voltageInput));

//  Serial.print("Voltage: ");
//  Serial.println(voltage);
  
  //We have voltage now we need resistance
  thermistorResistance = voltage * resistorValue / (voltageInput - voltage);
  
  kelvinTemp = 1 / (A + B * log(thermistorResistance) + C * (log(thermistorResistance) * log(thermistorResistance) * log(thermistorResistance)));
  celciusTemp = kelvinTemp - kelvinOffset;

  return celciusTemp;
}


float getTemp() {
  float temps[5];
  float totalTemp = 0;
  float currentTemp;
  for (int i = 0; i < 5; i++) {
    currentTemp = getInstantTemp();
    temps[i] = currentTemp;
    totalTemp += currentTemp;
  }
  float med = QuickMedian<float>::GetMedian(temps, 5);
  float mean = totalTemp / 5;
  return med;
}


void setRPM(float value) {
  setpointRPM = value;
  Serial.print("Set RPM to: ");
  Serial.println(setpointRPM);
}


void getFrequency() {
  if (digitalRead(squareWavePin) == HIGH and oscillated == 0) {      
   counter++;
   oscillated = 1;
//    Serial.println("HIGH");
  }
  if (digitalRead(squareWavePin) == LOW and oscillated == 1) {
    oscillated = 0;
//     Serial.println("LOW");
  }
  if (loopCount >= 10) {
    Serial.print("Counter: ");
    Serial.println(counter);
    frequency = counter / 2; //floor() check /10 or /2
    counter = 0;
    loopCount = 0;
    Serial.print("Frequency in function: ");
    Serial.println(frequency);
  }
}


float getRPM() {
  // frequency = 100.5;
  // getFrequency();
  Serial.println("Freq ="+String(frequency));
  float rpm = frequency * 30;
//
//  rpms[loopCount] = rpm;
//
//  if (loopCount == 4) {
//    float rpm = QuickMedian<float>::GetMedian(rpms, 5);
//    loopCount = 0;
//  }
 
  return rpm;
}


void setPH(float value) {
  setpointPH = value;
  Serial.print("Set pH to: ");
  Serial.println(setpointPH);
}


float getPH() {
  return 0.0;
  probeRead=analogRead(A1);
  voltage2=probeRead/1023.0*5.0;
  Serial.print("Voltage:");
  Serial.println(voltage2);
  //Convert voltage to pH
  pH = Z * voltage2 + Y;
  Serial.println(pH);

  return pH;

  
  float sensorValue = analogRead(A1);
  float phvalue=map(sensorValue,484,545,4,10);


   //calibrating when ph=7
   if (sensorValue>502 and sensorValue<515)
   {
   phvalue = 7.0;
   acid = false;
   alkaline= false;
   }
   //calibrating when ph=10 
   if (sensorValue>(540) and sensorValue<(545)){
   phvalue = 10.0;
   acid = true;
   alkaline = false;
   }
   //calibration of ph=4
   if (sensorValue>479 and sensorValue<484)
   {
   phvalue = 4.0;
   acid = false;
   alkaline = true;
   }
   //calibration of ph=5

   if (sensorValue>487 and sensorValue<494)
   {
   phvalue = 5.0;
   acid = false;
   alkaline = false;
   }
   
  Serial.print("PH probe value = ");
  Serial.println(sensorValue);
  Serial.print("PH reading = ");
  Serial.println(phvalue);

  return phvalue;
}


void phControl() {
  float ph = getPH();
  if (ph>(7.00)){
    digitalWrite(pump1,HIGH);
    digitalWrite(pump2,LOW);
  }

  if (ph<(5.00)){
    digitalWrite(pump2,HIGH);
    digitalWrite(pump1,LOW);
  }
}


void tempControl() {
  celciusTemp = getTemp();

  heaterOutputPower = 0;
  tempDifference = setpointTemperature - celciusTemp;

  Serial.print("Celcius Temp: ");
  Serial.println(celciusTemp);
  
  //If the temperature is less than the required temp, turn on the heating circuit
  if (celciusTemp < setpointTemperature) {
    // Power between 0 -> 255. Power should not exceed 204 due to heater design limitations.
    // Semi bang bang solution. Not purely proportional, allows for different temp outputs.

    if (tempDifference > 2)
      heaterOutputPower = 170;
    else if (tempDifference < 2 && tempDifference > 0.5)
      heaterOutputPower = 130;
    else
      heaterOutputPower = 90;
    
    Serial.print("Heater On with power: ");
    Serial.println(heaterOutputPower);
 
    analogWrite(heaterOutputPin,heaterOutputPower); 
    digitalWrite(heaterLEDpin, HIGH);
  } else {
    //Turn off the heater
    Serial.println("Heater off");
    digitalWrite(heaterLEDpin, LOW);
    digitalWrite(heaterOutputPin, LOW);
  }
}


void motorControl() {
  Serial.println("-----------------------------");

  float rpm = getRPM();
  Serial.println(String(rpm)+" rpm");

  if (rpm < (setpointRPM - 20) && motorspeed < 250) {
    motorspeed++;
  }
  if (rpm > (setpointRPM + 20)) {
    motorspeed--;
  }
  
  analogWrite(motorpin, motorspeed);
  Serial.print(motorspeed);
  Serial.println(" motor speed");

  Serial.println("************");
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
    setTemp(value);
  } else if (sensorNo == MOTOR) {
    setRPM(value);
  } else if (sensorNo == PH) {
    setPH(value);
  }
}


void death() {
  //Turn off the heater
  digitalWrite(heaterLEDpin, LOW);
  digitalWrite(heaterOutputPin, LOW);
  Serial.println("Heater off");

  // Turn off the motor
  analogWrite(motorpin, 0);
  Serial.println("Motor off");
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

  // Motor
  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);
  
  // Begin debug Serial Monitor
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println("I2C Slave Arduino");
}

void loop() {
  // delay(DELAY_TIME);
  // Handle control of sensors
  tempControl();
  Serial.print("Setpoint Temp: ");
  Serial.println(setpointTemperature);

  int new_counter = 0;
  time1 = millis();
  while (millis() - time1 < 1000) {
    new_counter++;
    if (digitalRead(squareWavePin) == HIGH and oscillated == 0) {      
   counter++;
   oscillated = 1;
//    Serial.println("HIGH");
  }
  if (digitalRead(squareWavePin) == LOW and oscillated == 1) {
    oscillated = 0;
//     Serial.println("LOW");
  }
  }

  Serial.print("New Counter: ");
  Serial.println(new_counter);

  Serial.print("Counter: ");
  Serial.println(counter);
  frequency = counter / 2; //floor() check /10 or /2
  counter = 0;
  Serial.print("Frequency in function: ");
  Serial.println(frequency);
  
  motorControl();
  loopCount++;
  Serial.print("Loop Count: ");
  Serial.println(loopCount);

  // phControl();
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
    sensorNo = atoi(token);
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
    death();
    while (1);
  }
}
