//https://circuitdigest.com/microcontroller-projects/arduino-ph-meter
float calibration_value = 21.34;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;

float voltageInput = 5.0;
int range = 1024;
float phVal;
void setup() 
{
 Serial.begin(9600);
}

float getph()
{
  for(int i=0;i<10;i++) 
 { 
 buffer_arr[i]=analogRead(A0);
 delay(30);
 }
 //Sorting alrgorithm
 for(int i=0;i<9;i++)
 {
   for(int j=i+1;j<10;j++)
   {
     if(buffer_arr[i]>buffer_arr[j])
     {
       temp=buffer_arr[i];
       buffer_arr[i]=buffer_arr[j];
       buffer_arr[j]=temp;
     }
   }
 }
 avgval=0;
 for(int i=2;i<8;i++)
  avgval+=buffer_arr[i];
 float volt=(float)avgval*voltageInput/range/6;
 float ph_act = -5.70 * volt + calibration_value;
 return ph_act
  
}
void loop() {
 phVal = getph();
 Serial.print("pHVal is: "+String(phVal));
 delay(1000);
}
