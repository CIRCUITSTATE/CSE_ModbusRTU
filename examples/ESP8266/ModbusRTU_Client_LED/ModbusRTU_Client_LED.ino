
//===================================================================================//
/*
  Filename: ModbusRTU_Client_LED.ino [ESP8266]
  Description: This example demonstrates the use of a Modbus RTU client to control an LED
  connected to a remote Modbus RTU server. The client generates periodic requests
  to the server to turn on and off the LED. The request consists of writing a coil
  located at address 0x0000. You can use the ModbusRTU_Server_LED example to run
  the server. We are using hardware serial port for the RS-485 interface.

  This code was tested with the NodeMCU-ESP12E ESP8266 board. For ModbusRTU emulation,
  you can use the Modbus Mechanic (https://github.com/SciFiDryer/ModbusMechanic) software.
  
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Library Version: 0.0.9
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_ModbusRTU
  Last Modified: +05:30 19:11:08 PM 28-05-2025, Wednesday
 */
//===================================================================================//

#include <CSE_ArduinoRS485.h>
#include <CSE_ModbusRTU.h>

//===================================================================================//

// You can define the serial port pins here.
#define PIN_RS485_RX        14
#define PIN_RS485_TX        12

#define PORT_RS485          softSerial // The software serial port for the RS-485 interface

//===================================================================================//

// Use the software serial port for the RS485 interface and the hardware serial port for
// the debugging output.
SoftwareSerial softSerial (PIN_RS485_RX, PIN_RS485_TX); // RX, TX

// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, -1, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)

// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x02, "modbusRTU-0x02"); // (RS-485 Port, Device Address, Device Name)

// Create a Modbus RTU client instance with the Modbus RTU node.
CSE_ModbusRTU_Client modbusRTUClient (modbusRTU, "modbusRTUClient"); // (CSE_ModbusRTU, Client Name)

//===================================================================================//

void setup() {
  // Initialize the default serial port for debug messages.
  Serial.begin (115200);
  delay (1000);
  Serial.println ("CSE_ModbusRTU - Modbus RTU Client LED");

  // Initialize the RS485 port manually.
  // This particular begin() call is specific to ESP8266-Arduino.
  PORT_RS485.begin (9600, SWSERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);

  // Initialize the RS485 interface. If you are initializing the RS485 interface
  // manually, then the parameter can be empty.
  RS485.begin();

  // Initialize the Modbus RTU client.
  modbusRTUClient.begin();
  modbusRTUClient.setServerAddress (0x01); // Set the server address to 0x01
}

//===================================================================================//

void loop() {
  if (modbusRTUClient.writeCoil (0x00, 0xFF00) == -1) { // Turn the LED on
    Serial.println ("Turning on the LED failed.");
  }
  else {
    Serial.println ("Turning on the LED successful.");
  }

  delay (1000);

  if (modbusRTUClient.writeCoil (0x00, 0x0000) == -1) { // Turn the LED off
    Serial.println ("Turning off the LED failed.");
  }
  else {
    Serial.println ("Turning off the LED successful.");
  }
  delay (1000);
}

//===================================================================================//
