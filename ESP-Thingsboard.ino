#include "ThingsBoard.h"
#include <WiFi.h>

#define WIFI_AP "Me"
#define WIFI_PASSWORD "MeMyself"

#define TOKEN "n8cBswJGipnRCD0AJVte"
#define THINGSBOARD_SERVER "demo.thingsboard.io"
const int LED_PIN = 25;
int temperature_value;

#define SERIAL_DEBUG_BAUD   115200

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
    return RPC_Response(NULL, temperature_value);
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

const size_t callbacks_size = 2;
RPC_Callback callbacks[callbacks_size] = {
  { "setValue", processSetValue }, { "setTemp", processSetTemp }
};

bool subscribed = false;

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  InitWiFi();
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  delay(1000);
  
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
  tb.sendTelemetryInt("temperature", temperature_value);
  tb.sendTelemetryFloat("pH", 6.6);
  tb.sendTelemetryFloat("Motor stirring rate", 1000);

  Serial.println("Waiting for data...");
  tb.loop();
}
