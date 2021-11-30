float v;
float voltage;
int squareWavePin = 34;
unsigned long lastTime;
unsigned long myTime;
int counter = 0;
int frequency;
int oscillated = 0;
void setup() {
  Serial.begin(9600);
  pinMode(squareWavePin, INPUT);
}
//http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm

int getFrequency()
{
  myTime = millis();
  lastTime = millis();
  while(myTime - lastTime < 1000)
  {
    myTime = millis();
    if (digitalRead(squareWavePin) == HIGH and oscillated == 0)
    {      
     counter++;
     oscillated = 1;
     Serial.println("HIGH");
    }
    if (digitalRead(squareWavePin) == LOW and oscillated == 1)
    {
      oscillated = 0;
      Serial.println("LOW");
    }
  }
   frequency = floor(counter/2);
   counter = 0;
   lastTime = myTime;
   Serial.print("Frequency in function"+frequency);
   return frequency;
 

}
void loop() {
 Serial.println("Freq ="+String(getFrequency()));
  
  
}
