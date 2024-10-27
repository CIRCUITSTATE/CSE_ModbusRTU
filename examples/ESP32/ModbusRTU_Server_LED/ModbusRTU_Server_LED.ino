
//===================================================================================//
/**
 * @file ModbusRTU_Server_LED.ino
 * @brief This example demonstrates the use of a Modbus RTU server to listen for Modbus
 * requests and turn an LED on and off. The request can come from any Modbus RTU client
 * including the ModbusRTU_Client_LED example. We define 4 coils but only use the 0x00
 * one to turn the LED on and off. We are using hardware serial port for the RS-485
 * interface.
 * 
 * This code was tested with the DOIT-ESP32-DevKit-V1 board.
 * 
 * @date +05:30 04:45:28 PM 02-08-2023, Wednesday
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
#define PIN_RS485_RX        16
#define PIN_RS485_TX        17

#define PORT_RS485          Serial1 // The hardware serial port for the RS-485 interface

//===================================================================================//

const int ledPin = LED_BUILTIN;

// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, 3, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)

// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x01, "modbusRTU-0x01"); // (RS-485 Port, Device Address, Device Name)

// Create a Modbus RTU server instance with the Modbus RTU node.
CSE_ModbusRTU_Server modbusRTUServer (modbusRTU, "modbusRTUServer"); // (CSE_ModbusRTU, Server Name)

//===================================================================================//

void setup() {
  // Initialize the default serial port for debug messages
  Serial.begin (115200);
  delay (1000);
  Serial.println ("CSE_ModbusRTU - Modbus RTU Server LED");

  // Initialize the RS485 port manually.
  // This particualr begin() call is specific to ESP32-Arduino.
  // If you are using a different controller, change the begin() call accordingly.
  // PORT_RS485.begin (9600, SERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);

  PORT_RS485.setRX (1);
  PORT_RS485.setTX (0);

  pinMode (3, OUTPUT);

  // Initialize the RS485 interface. If you are initializing the RS485 interface
  // manually, then the parameter can be empty.
  RS485.begin (9600);

  // Initialize the Modbus RTU server
  modbusRTUServer.begin();

  // Configure the LED
  pinMode (ledPin, OUTPUT);
  digitalWrite (ledPin, LOW);

  // Configure four coils starting at address 0x00
  // modbusRTUServer.configureCoils (0x00, 4);
  modbusRTUServer.configureHoldingRegisters (0x00, 9);
}

//===================================================================================//

void loop() {
  // Poll for Modbus RTU requests
  int requestReceived = modbusRTUServer.poll();

  if ((requestReceived != -1) && (requestReceived < 0x80)) {
    Serial.println ("Request received");
    // Read the current value of the coil
    int coilValue = modbusRTUServer.readCoil (0x00);
  
    if (coilValue == 1) {
      // Coil value set, turn LED on
      digitalWrite (ledPin, HIGH);
    }
    else if (coilValue == 0) {
      // Coil value clear, turn LED off
      digitalWrite (ledPin, LOW);
    }
    else {
      Serial.println ("Error reading coil");
    }
  }
}

//===================================================================================//
