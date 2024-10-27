
//===================================================================================//
/**
 * @file ModbusRTU_AVR_Server_LED.ino
 * @brief This example demonstrates the use of a Modbus RTU server to listen for Modbus
 * requests and turn an LED on and off. The request can come from any Modbus RTU client
 * including the ModbusRTU_AVR_Client_LED example. We define 4 coils but only use the 0x00
 * one to turn the LED on and off. We are using hardware serial port for the RS-485
 * interface.
 * 
 * This code was specifically written for AVR boards and was tested with the
 * Arduino Uno board.
 * 
 * @date +05:30 02:34:41 PM 28-01-2024, Sunday
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_ModbusRTU
 * @par MIT License
 * 
 */
//===================================================================================//

#include <CSE_ArduinoRS485.h>
#include <CSE_ModbusRTU.h>
#include <SoftwareSerial.h>

//===================================================================================//

// You can define the software serial port pins here.
#define PIN_RS485_RX        8
#define PIN_RS485_TX        9

#define PORT_RS485          Serial2 // The software serial port for the RS-485 interface

//===================================================================================//

// Create a new software serial port.
SoftwareSerial Serial2 (PIN_RS485_RX, PIN_RS485_TX);

const int ledPin = LED_BUILTIN;

// Declare the RS485 interface here with a software serial port.
RS485Class RS485 (PORT_RS485, -1, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)

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
  PORT_RS485.begin (9600);

  // Initialize the RS485 interface. If you are initializing the RS485 interface
  // manually, then the parameter can be empty.
  RS485.begin();

  // Initialize the Modbus RTU server
  modbusRTUServer.begin();

  // Configure the LED
  pinMode (ledPin, OUTPUT);
  digitalWrite (ledPin, LOW);

  // Configure four coils starting at address 0x00
  modbusRTUServer.configureCoils (0x00, 4);
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
