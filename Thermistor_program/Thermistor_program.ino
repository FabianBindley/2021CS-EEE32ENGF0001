int x=0;
int L1=3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(L1,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  x=analogRead(A0);
  Serial.println(x);
  delay(100);
 
  if (x<84.26) 
  //V(out) of thermistor reach 84.26 when temperature reach 25 degree, so turn on heater when V below 25 degree:
  {
    digitalWrite(L1,HIGH);
    delay(100);
  }
  else
  {
    digitalWrite(L1,LOW);
  }
  
  }
