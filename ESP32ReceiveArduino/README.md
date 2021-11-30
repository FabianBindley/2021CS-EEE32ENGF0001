
# ESP32 to Arduino Communication Protocol

This protocol defines how instructions are sent to/from the ESP32 and Arduino board, using the Wire library and the I2C protocol.

**__Sensors__**

```
1 - Heating element (°C)
2 - Motor (RPM)
3 - pH pumps (pH)
```

**__Instruction Set__**
```
0 - READ <Sensor Number> - Reads the current value for the specified sensor, with output being returned in the correct unit.
Returns - Output (ASCII encoded)

1 - SET <Sensor Number> <Value (Heating °C/Motor RPM/pH pH)> - Issues a set command, to change the value of a sensor. It should pass a value in the correct unit, specified above per sensor.
e.g. SET 1 38
Returns - 1 if successful else 0

2 - STOP - Issues a stop, raises SysExit for Arduino board.
Returns - 1 if successful else 0
```
If an invalid instruction is entered, an error is raised and no data is transmitted.

**__Encoding__**

All data is sent with ASCII encoding as bytes. To avoid unnecessary data transmission, and to reduce the overhead of parsing raw signal instructions, the transmission handles conversion of instructions in the following manner:

1. Convert each of the instructions into the corresponding number. Since each instruction has a pre-specified number of arguments, the number of arguments to be read from the input can be determined from just the instruction.
2. Pass each argument (if any), separated with a space.  

**__Decoding__**

As according to the instruction set mapping above, the Arduino receives numbers corresponding to commands it should handle. On this side, the process should be as follows:

1. Receive each argument as bytes.
2. Compare against the internal instruction set to call the appropriate code.
3. Return any values through the same serial port.  

A protocol outlined like above may also accept inputs through other means. For example, instead of listening on a serial port, a webhook listener may be implemented allowing inbound commands from the internet instead.