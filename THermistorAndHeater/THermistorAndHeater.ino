int vInput=0;
int heaterOutputPin=3;
int thermistorInputPin = A0;
int heaterLEDpin = 4;
float voltage;
int range = 1024;
float voltageInput = 5;
int resistorValue = 10000;
float kelvinTemp;
float celciusTemp;
float setpointTemperature = 34.0;
float temperatureLagOffset = 0;
float thermistorResistance;
float tempDifference;
int heaterOutputPower;


int targetspeed =600;
float v;
int squareWavePin = 2;
int motorpin = 6;
int motorspeed =  40; //note buzzing (this keeps changing?)
unsigned long lastTime;
unsigned long myTime;
int counter = 0;
float frequency; //back to int?
int rpm;
int oscillated = 0;


const float A = 0.0012323996168659172;
const float B = 0.00022347497515516762;
const float C = 8.354794023823214e-08;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(heaterOutputPin,OUTPUT);
  pinMode(heaterLEDpin,OUTPUT);
  pinMode(thermistorInputPin,INPUT);
  pinMode(squareWavePin, INPUT);
}


float getFrequency()
{
  myTime = millis();
  lastTime = millis();
  Serial.println(myTime);
  while(myTime - lastTime < 1000) //check duration
  {
    myTime = millis();
    if (digitalRead(squareWavePin) == HIGH and oscillated == 0)
    {      
     counter++;
     oscillated = 1;
 //    Serial.println("HIGH");
    }
    if (digitalRead(squareWavePin) == LOW and oscillated == 1)
    {
      oscillated = 0;
 //     Serial.println("LOW");
    }
  }
  Serial.println("Counter"+String(counter));
   frequency = counter/2; //floor() check /10 or /2
   counter = 0;
   lastTime = myTime;
   Serial.print("Frequency in function"+String(frequency));
   return frequency;
}

int correctmotor()
{
  rpm = frequency * 30;
  if (rpm < (targetspeed -20) && motorspeed < 250)
  {
    motorspeed += 1;
  }
  if (rpm > (targetspeed +20))
  {
    motorspeed -= 1;
  }
}

 
void loop() {
  /*
  vInput=analogRead(thermistorInputPin);
  delay(100);
  voltage = voltageInput - vInput/(range/voltageInput);
  Serial.println("Voltage:"+String(voltage));
  
  //We have voltage now we need resistance
  thermistorResistance = voltage * resistorValue/(voltageInput-voltage);
  
  kelvinTemp = 1/(A+B*log(thermistorResistance)+C*(log(thermistorResistance)*log(thermistorResistance)*log(thermistorResistance)));
  celciusTemp = kelvinTemp - 273.15;
  
  Serial.println("Celcius Temp: "+String(celciusTemp));

 
  
  heaterOutputPower = 0;
  tempDifference =  setpointTemperature - celciusTemp;

  Serial.println("Difference is"+ String(tempDifference));
  if (celciusTemp<setpointTemperature) 
  //If the temperature is less than the required temp - the lag offset, turn on the heating circuit
  {
    
    //Power between 0 -> 255. Power should not exceed 204 due to heater design limitations.
    //Semi bang bang solution. Not purely proportional, allows for different temp outputs.
    if (tempDifference>2)
      heaterOutputPower = 170;
    else if (tempDifference<2 && tempDifference > 0.5)
      heaterOutputPower = 130;
    else
      heaterOutputPower = 90;
    Serial.println("Heater On with power: "+String(heaterOutputPower));
    analogWrite(heaterOutputPin,heaterOutputPower); 
    digitalWrite(heaterLEDpin, HIGH);
    
  }
  else
  {
    //Turn off the heater
    Serial.println("Heater off");
    digitalWrite(heaterLEDpin, LOW);
    digitalWrite(heaterOutputPin,LOW);
  }*/
  Serial.println("-----------------------------");
  frequency = getFrequency();
  Serial.println("Freq ="+String(frequency));
  Serial.println(String(rpm)+" rpm");
  correctmotor();
  analogWrite(motorpin,motorspeed);
  Serial.println(String(motorspeed)+" motor speed");
  Serial.println("************");
  
}
