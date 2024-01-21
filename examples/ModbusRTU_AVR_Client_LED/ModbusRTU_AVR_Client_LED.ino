
//===================================================================================//
/**
 * @file ModbusRTU_AVR_Client_LED.ino
 * @brief This example demonstrates the use of a Modbus RTU client to control an LED
 * connected to a remote Modbus RTU server. The client generates periodic requests
 * to the server to turn on and off the LED. The request consists of writing a coil
 * located at address 0x0000. You can use the ModbusRTU_AVR_Server_LED example to run
 * the server. We are using hardware serial port for the RS-485 interface.
 * 
 * This code was tested with the Arduino Uno board.
 * 
 * @date +05:30 07:12:58 PM 21-01-2024, Sunday
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

// Declare the RS485 interface here with a software serial port.
RS485Class RS485 (PORT_RS485, -1, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)

// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x02, "modbusRTU-0x02"); // (RS-485 Port, Device Address, Device Name)

// Create a Modbus RTU server instance with the Modbus RTU node.
CSE_ModbusRTU_Client modbusRTUClient (modbusRTU, "modbusRTUClient"); // (CSE_ModbusRTU, Client Name)

//===================================================================================//

void setup() {
  // Initialize the default serial port for debug messages
  Serial.begin (115200);
  delay (1000);
  Serial.println ("CSE_ModbusRTU - Modbus RTU Client LED");

  // Initialize the RS485 port manually.
  PORT_RS485.begin (9600);

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
    Serial.println ("Turning LED on failed.");
  }
  else {
    Serial.println ("Turning LED on success.");
  }

  delay (1000);

  if (modbusRTUClient.writeCoil (0x00, 0x0000) == -1) { // Turn the LED off
    Serial.println ("Turning LED off failed.");
  }
  else {
    Serial.println ("Turning LED off success.");
  }
  delay (1000);
}

//===================================================================================//
