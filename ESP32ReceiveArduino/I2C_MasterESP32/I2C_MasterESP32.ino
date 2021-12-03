// ESP 32 Serial read program.
// https://www.arduino.cc/reference/en/language/functions/communication/serial/read/ - Some of the program inspired from here
// https://icircuit.net/arduino-interfacing-arduino-uno-esp32/2134 - Serial transmission circuit diagram from here

#include <string.h>
#include <Wire.h>

#include "ThingsBoard.h"
#include <WiFi.h>

#define SERIAL_DEBUG_BAUD 115200

// Define the pins used for SDA and SCL
#define I2C_SCL 21
#define I2C_SDA 22

#define INPUT_SIZE 10
#define SLAVE_ADDR 9  // Define Slave I2C Address

#define WIFI_AP "Me"
#define WIFI_PASSWORD "MeMyself"

#define TOKEN "n8cBswJGipnRCD0AJVte"
#define THINGSBOARD_SERVER "demo.thingsboard.io"


const int LED_PIN = 25;
bool subscribed = false;

float temperature_value;
float ph_value;
float motor_value;

char read_temp[] = "READ 1";
char read_motor[] = "READ 2";
char read_ph[] = "READ 3";


WiFiClient espClient;
ThingsBoard tb(espClient);


void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
}

RPC_Response processSetTemp(const RPC_Data &data) {
    Serial.println("Received the set Temp RPC method");
    temperature_value = data;
    Serial.print("Set new value: ");
    Serial.println(temperature_value);

    char str_temp[8];
    char set_str[16];
    dtostrf(temperature_value, 4, 2, str_temp);
    sprintf(set_str, "SET 1 %s", str_temp);
    Serial.println(set_str);
    write_arduino(set_str);

    return RPC_Response(NULL, temperature_value);
}

RPC_Response processSetPH(const RPC_Data &data) {
    Serial.println("Received the set pH RPC method");
    ph_value = data;
    Serial.print("Set new value: ");
    Serial.println(ph_value);

    char str_ph[8];
    char set_str[16];
    dtostrf(ph_value, 4, 2, str_ph);
    sprintf(set_str, "SET 3 %s", str_ph);
    Serial.println(set_str);
    write_arduino(set_str);

    return RPC_Response(NULL, ph_value);
}

RPC_Response processSetMotor(const RPC_Data &data) {
    Serial.println("Received the set Motor RPC method");
    motor_value = data;
    Serial.print("Set new value: ");
    Serial.println(motor_value);

    char str_motor[8];
    char set_str[16];
    dtostrf(motor_value, 4, 2, str_motor);
    sprintf(set_str, "SET 2 %s", str_motor);
    Serial.println(set_str);
    write_arduino(set_str);

    return RPC_Response(NULL, motor_value);
}

RPC_Response processGetValue( const RPC_Data &data) {
  Serial.println("Recieved the get value method");
  
}

RPC_Response processSetValue (const RPC_Data &data) {
  Serial.println("Recieved the set value method");
  bool led_state = data;
  Serial.println(led_state);
  if (led_state == false) {
    digitalWrite(LED_PIN, LOW);
  }
  if (led_state == true) {
    digitalWrite(LED_PIN, HIGH);
  }
  return RPC_Response(NULL, led_state);
}

const size_t callbacks_size = 4;
RPC_Callback callbacks[callbacks_size] = {
  { "setBioreactor", processSetValue }, { "setTemp", processSetTemp },
  { "setPH", processSetPH }, { "setMotor", processSetMotor }
};


// Compares the input instruction and returns a corresponding int
int encode_instr(char *arg) {
  if (strcasecmp(arg, "READ") == 0) {
    return 0;
  } else if (strcmp(arg, "SET") == 0) {
    return 1;
  } else if (strcmp(arg, "STOP") == 0) {
    return 2;
  } else {
    return -1;
  }
}


void write_arduino(char *str) {
  Wire.beginTransmission(SLAVE_ADDR); // Transmit to specified device
  Wire.write(str);
  Wire.endTransmission(); 
}


float read_arduino() {
  String received_string = "";

  Wire.requestFrom(SLAVE_ADDR, 6);
  while (Wire.available()) {
    received_string += Wire.read();
  }
  Serial.println(received_string);

  float result = received_string.toFloat();

  return result;
}


void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(SERIAL_DEBUG_BAUD); // opens Serial port and set baud rate
  InitWiFi();
  pinMode(LED_PIN, OUTPUT);
}


void loop() {
  Serial.println("Waiting for data...");
  delay(1000);

  // Thingsboard connection
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }
  if (!tb.connected()) {
    subscribed = false;
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect, retrying....");
      return;
    }
  }   
  
  if (!subscribed) {
    Serial.println("Subscribing for RPC...");
    if (!tb.RPC_Subscribe(callbacks, callbacks_size)) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }

  Serial.println("Sending data....");

  write_arduino(read_temp);
  float temp = read_arduino();
  // float temp = 1.23;
//  tb.sendTelemetryFloat("temperature", temp);
//
//  write_arduino(read_ph);
//  float ph = read_arduino();
//  // float ph = 2.34;
//  tb.sendTelemetryFloat("pH", ph);
//
//  write_arduino(read_motor);
//  float motor = read_arduino();
//  // float motor = 3.45;
//  tb.sendTelemetryFloat("Motor stirring rate", motor);

  tb.loop();
}
