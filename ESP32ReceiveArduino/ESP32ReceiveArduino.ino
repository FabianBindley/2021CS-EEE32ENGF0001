// ESP 32 Serial read program.
// https://www.arduino.cc/reference/en/language/functions/communication/serial/read/ - Some of the program inspired from here
// https://icircuit.net/arduino-interfacing-arduino-uno-esp32/2134 - Serial transmission circuit diagram from here

#include <string.h>
#include <Wire.h>


// Define the pins used for SDA and SCL
#define I2C_SDA 21
#define I2C_SCL 22

#define INPUT_SIZE 10
#define SLAVE_ADDR 9  // Define Slave I2C Address


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


void read_arduino() {
  char received_string[6];
  int counter = 0;

  Wire.requestFrom(SLAVE_ADDR, 6);
  while (Wire.available()) {
    received_string[counter++] = Wire.read();
  }

  Serial.println(received_string);
}


void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(9600); // opens Serial port and set baud rate to 9600
}


void loop() {
  // Return early if there's no data on the Serial port
  if (Serial.available() < 1) {
    return;
  }

  // Get next command from Serial (add 1 for final 0)
  char input[INPUT_SIZE + 1];
  byte size = Serial.readBytes(input, INPUT_SIZE);
  
  // Add the final 0 to end the C string
  input[size] = 0;

  // Declare vars for instruction and arguments
  int instr;
  char *arg = strtok(input, " ");

  // Assign the instr and check if it is valid
  instr = encode_instr(arg);
  if (instr == -1) {
    Serial.println("Invalid input.");
  } else if (instr == 0) {
    read_arduino();
  } else {
    write_wire(input);
  }
}
