
//===================================================================================//
/*
  Filename: Holding_Register_Client.ino [RP2040]
  Description: This example demonstrates how to read/write Holding Registers at a Server.
  You can use the `Holding_Register_Server.ino` sketch on the Server side to respond
  to the requests from the Client.

  This code was tested with the Raspberry Pi Pico RP2040 board. For ModbusRTU emulation,
  you can use the Modbus Mechanic (https://github.com/SciFiDryer/ModbusMechanic) software.
  
  Framework: Arduino, PlatformIO
  Author: Vishnu Mohanan (@vishnumaiea, @vizmohanan)
  Maintainer: CIRCUITSTATE Electronics (@circuitstate)
  Version: 0.0.8
  License: MIT
  Source: https://github.com/CIRCUITSTATE/CSE_ModbusRTU
  Last Modified: +05:30 15:18:56 PM 23-03-2025, Sunday
 */
//===================================================================================//

#include <CSE_ArduinoRS485.h>
#include <CSE_ModbusRTU.h>

//===================================================================================//
// Macros and constants

// You can define the serial port pins here.
#define   PIN_RS485_DE        2
#define   PIN_RS485_RE        3
#define   PIN_RS485_RX        1
#define   PIN_RS485_TX        0

#define   PORT_RS485          Serial1 // The hardware serial port for the RS-485 interface

//===================================================================================//
// Globals

// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, PIN_RS485_DE, PIN_RS485_RE, PIN_RS485_TX); // Port, DE, RE, TX

// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x02, "modbusRTU-0x02"); // (RS-485 Port, Device Address, Device Name)

// Create a Modbus RTU client instance with the Modbus RTU node.
CSE_ModbusRTU_Client modbusRTUClient (modbusRTU, "modbusRTUClient"); // (CSE_ModbusRTU, Client Name)

// Create an array to hold the values read from the Server.
uint16_t holdingRegisters [10] = {0};

//===================================================================================//
void setup() {
  // Initialize the default serial port for debug messages.
  Serial.begin (115200);
  delay (1000);
  Serial.println ("CSE_ModbusRTU - Holding Register Client");

  // // Optional for RP2040. Set the serial port pins here.
  // PORT_RS485.setRX (PIN_RS485_RX);
  // PORT_RS485.setTX (PIN_RS485_TX);

  // Initialize the RS485 port manually.
  // This particualr begin() call is specific to RP2040-Arduino.
  // If you are using a different controller, change the begin() call accordingly.
  PORT_RS485.begin (9600, SERIAL_8N1);

  // Initialize the RS485 interface. If you are initializing the RS485 interface
  // manually, then the parameter can be empty.
  RS485.begin();

  // Initialize the Modbus RTU client.
  modbusRTUClient.begin();

  // Set the address of the Server you want to communicate with.
  // You can change this anytime to communicate with a new Server.
  modbusRTUClient.setServerAddress (0x01);

  // Enable/Disable the debug messages here.
  // CSE_ModbusRTU_Debug:: enableDebugMessages();
  CSE_ModbusRTU_Debug:: disableDebugMessages();
}

//===================================================================================//

void loop() {
  // Read a single Holding Register.
  Serial.println ("Reading a single Holding Register at 0x00..");

  holdingRegisters [0] = 0;

  if (modbusRTUClient.readHoldingRegister (0x00, 1, holdingRegisters) == -1) {
    Serial.println ("Reading the Holding Register failed.");
  }
  else {
    Serial.print ("Reading the Holding Register successful. Value = 0x");
    Serial.println (holdingRegisters [0], HEX);
  }

  Serial.println();
  delay (500);

  //-----------------------------------------------------------------------------------//

  // Read multiple Holding Registers.
  Serial.println ("Reading multiple Holding Registers starting at 0x02..");

  holdingRegisters [0] = 0;
  holdingRegisters [1] = 0;

  if (modbusRTUClient.readHoldingRegister (0x02, 2, holdingRegisters) == -1) {
    Serial.println ("Reading the Holding Registers failed.");
  }
  else {
    Serial.print ("Reading the Holding Registers successful. Value = 0x");
    Serial.print (holdingRegisters [0], HEX);
    Serial.print (", 0x");
    Serial.println (holdingRegisters [1], HEX);
  }

  Serial.println();
  delay (500);

  //-----------------------------------------------------------------------------------//

  // Write a single Holding Register.
  Serial.println ("Writing a single Holding Register at 0x03 to 0x1234..");

  if (modbusRTUClient.writeHoldingRegister (0x03, 0x1234) == -1) {
    Serial.println ("Writing the Holding Register failed.");
  }
  else {
    Serial.println ("Writing the Holding Register successful.");
  }

  Serial.println();
  delay (500);

  //-----------------------------------------------------------------------------------//

  // Read the value back from the Server.
  Serial.println ("Reading back the Holding Register at 0x03..");

  holdingRegisters [0] = 0;

  if (modbusRTUClient.readHoldingRegister (0x03, 1, holdingRegisters) == -1) {
    Serial.println ("Reading the Holding Register failed.");
  }
  else {
    Serial.print ("Reading the Holding Register successful. Value = 0x");
    Serial.println (holdingRegisters [0], HEX);
  }

  // Restore the old value.
  Serial.println ("Restoring the Holding Register at 0x03 to 0x00FF..");
  if (modbusRTUClient.writeHoldingRegister (0x03, 0x00FF) == -1) {
    Serial.println ("Writing the Holding Register failed.");
  }
  else {
    Serial.println ("Writing the Holding Register successful.");
  }

  Serial.println();
  delay (500);

  //-----------------------------------------------------------------------------------//

  // Write multiple Holding Registers.
  Serial.println ("Writing multiple Holding Registers starting at 0x02 with values 0x1717 & 0xF00F..");

  holdingRegisters [0] = 0x1717;
  holdingRegisters [1] = 0xF00F;

  if (modbusRTUClient.writeHoldingRegister (0x02, 2, holdingRegisters) == -1) {
    Serial.println ("Writing the Holding Registers failed.");
  }
  else {
    Serial.println ("Writing the Holding Registers successful.");
  }

  Serial.println();
  delay (500);

  //-----------------------------------------------------------------------------------//

  // Read the values back from the Server.
  holdingRegisters [0] = 0;
  holdingRegisters [1] = 0;

  Serial.println ("Reading back the Holding Registers at 0x02..");
  if (modbusRTUClient.readHoldingRegister (0x02, 2, holdingRegisters) == -1) {
    Serial.println ("Reading the Holding Registers failed.");
  }
  else {
    Serial.print ("Reading the Holding Registers successful. Value = 0x");
    Serial.print (holdingRegisters [0], HEX);
    Serial.print (", 0x");
    Serial.println (holdingRegisters [1], HEX);
  }

  // Restore the old values.
  holdingRegisters [0] = 0xFF00;
  holdingRegisters [1] = 0x00FF;

  Serial.println ("Restoring the Holding Registers at 0x02 with values 0xFF00 & 0x00FF..");
  if (modbusRTUClient.writeHoldingRegister (0x02, 2, holdingRegisters) == -1) {
    Serial.println ("Writing the Holding Registers failed.");
  }
  else {
    Serial.println ("Writing the Holding Registers successful.");
  }

  Serial.println();
  delay (500);
}

//===================================================================================//
