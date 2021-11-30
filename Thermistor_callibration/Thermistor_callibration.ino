float v;
float voltage;
int thermistorPin = 34;
//Max voltage is 3.3 v and max output is 4095
//1v = 1241
void setup() {
  Serial.begin(9600);
}
//http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm
void loop() {
  delay(1000);
  v = analogRead(thermistorPin);
  Serial.println("Read"+String(v));
  voltage = v/1241;
  Serial.println("Voltage:"+String(voltage));
}
