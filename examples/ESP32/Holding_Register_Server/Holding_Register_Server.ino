
//===================================================================================//
/**
  * @file Holding_Register_Server.ino
  * @brief This example demonstrates how to store Holding Registers at a Server
  * and respond to a Client's requests to read/write them. You can use the 
  * `Holding_Register_Client.ino` sketch on the Client side to test the server.
  * 
  * This code was tested with the DFRobot FireBeetle 2 board and Modbus Mechanic.
  * 
  * @date +05:30 06:23:38 PM 09-11-2024, Saturday
  * @author Vishnu Mohanan (@vishnumaiea)
  * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_ModbusRTU
  * @par MIT License
  * 
  */
//===================================================================================//

#include <CSE_ArduinoRS485.h>
#include <CSE_ModbusRTU.h>

//===================================================================================//

// You can define the serial port pins here.
#define   PIN_RS485_RX        16
#define   PIN_RS485_TX        17

#define   PORT_RS485          Serial1 // The hardware serial port for the RS-485 interface

//===================================================================================//

// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, -1, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)

// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x01, "modbusRTU-0x01"); // (RS-485 Port, Device Address, Device Name)

// Create a Modbus RTU server instance with the Modbus RTU node.
CSE_ModbusRTU_Server modbusRTUServer (modbusRTU, "modbusRTUServer"); // (CSE_ModbusRTU, Server Name)

//===================================================================================//
void setup() {
  // Initialize the default serial port for debug messages.
  Serial.begin (115200);
  delay (1000);
  Serial.println ("CSE_ModbusRTU - Holding Register Server");

  // Initialize the RS485 port manually.
  // This particualr begin() call is specific to ESP32-Arduino.
  // If you are using a different controller, change the begin() call accordingly.
  PORT_RS485.begin (9600, SERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);

  // Initialize the RS485 interface. If you are initializing the RS485 interface
  // manually, then the parameter can be empty.
  RS485.begin();

  // Initialize the Modbus RTU server.
  modbusRTUServer.begin();

  // Configure nine Holding Registers starting at address 0x00.
  modbusRTUServer.configureHoldingRegisters (0x00, 9);
  
  // Set the value of the first four Holding Registers.
  modbusRTUServer.writeHoldingRegister (0x00, 0x1234);
  modbusRTUServer.writeHoldingRegister (0x01, 0x4321);
  modbusRTUServer.writeHoldingRegister (0x02, 0xFF00);
  modbusRTUServer.writeHoldingRegister (0x03, 0x00FF);

  // Enable/Disable the debug messages here.
  CSE_ModbusRTU_Debug:: enableDebugMessages();
  // CSE_ModbusRTU_Debug:: disableDebugMessages();
}

//===================================================================================//

void loop() {
  // Poll for Modbus RTU requests.
  int requestReceived = modbusRTUServer.poll();
  delay (10)
}

//===================================================================================//