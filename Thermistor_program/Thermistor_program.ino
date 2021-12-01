int vInput=0;
int heaterOutputPin=3;
int thermistorInputPin = A0;
int range = 1024;
float voltageInput = 5;
int resistorValue = 10000;
float kelvinTemp;
float celciusTemp;

float setpointTemperature = 30.0;

const float A = ;
const float B = ;
const float C = ;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(heaterOutputPin,OUTPUT);
  pinMode(thermistorInputPin,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  vInput=analogRead(thermistorInputPin);
  Serial.println(x);
  delay(100);
  voltage = vInput/(range/voltageInput);
  Serial.println("Voltage:"+String(voltage));
  
  //We have voltage now we need resistance
  thermistorResistance = voltage * resistorValue/(voltageInput-voltage);
  
  kelvinTemp = 1/(A+B*log(thermistorResistance)+C*(log(thermistorResistance)*log(thermistorResistance)*log(thermistorResistance)))
  celciusTemp = kelvinTemp - 273.15;
  
  if (celciusTemp<25) 
  //V(out) of thermistor reach 84.26 when temperature reach 25 degree, so turn on heater when V below 25 degree:
  {
    //Turn on the heater for 10 seconds
    digitalWrite(heaterOutputPin, HIGH);
  }
  else
  {
    //Turn off the heater
    digitalWrite(heaterOutputPin,LOW);
  }
  
}
