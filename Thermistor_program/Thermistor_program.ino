int vInput=0;
int heaterOutputPin=3;
int thermistorInputPin = A0;
int heaterLEDpin = 4;

int range = 1024;
float voltageInput = 5;
int resistorValue = 10000;
float kelvinTemp;
float celciusTemp;

float setpointTemperature = 30.0;
float temperatureLagOffset = 1.0;

const float A = 0.0012323996168659172;
const float B = 0.00022347497515516762;
const float C = 8.354794023823214e-08;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(heaterOutputPin,OUTPUT);
  pinMode(heaterLEDpin,OUTPUT);
  pinMode(thermistorInputPin,INPUT);
}
float getTemperature()
{
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
  return celciusTemp;
}
void loop() {
  celciusTemp = getTemperature();
  Serial.println("Celcius Temp: "+celciusTemp);
  
  if (celciusTemp<setpointTemperature - temperatureLagOffset) 
  //If the temperature is less than the required temp - the lag offset, turn on the heating circuit
  {
    //Turn on the heater at half strength using 50% PWM
    analogWrite(heaterOutputPin,127); 
    digitalWrite(heaterLEDpin, HIGH);
    Serial.println("Heater On");
  }
  else
  {
    //Turn off the heater
    Serial.println("Heater off");
    digitalWrite(heaterLEDpin, LOW);
    digitalWrite(heaterOutputPin,LOW);
  }
  
}
