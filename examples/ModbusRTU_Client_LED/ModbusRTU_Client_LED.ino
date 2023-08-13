
//===================================================================================//
/**
 * @file ModbusRTU_Client_LED.ino
 * @brief This example demonstrates the use of a Modbus RTU server to listen for Modbus
 * requests and turn an LED on and off. The request can come from any Modbus RTU client
 * including the ModbusRTU_Client_LED example. We define 4 coils but only use the 0x00
 * one to turn the LED on and off. We are using hardware serial port for the RS-485
 * interface.
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

#define PORT_RS485          Serial2 // The hardware serial port for the RS-485 interface

//===================================================================================//

// Declare the RS485 interface here with a hardware serial port.
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

  // Initialize the RS485 port manually
  PORT_RS485.begin (9600, SERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);

  // Initialize the RS485 interface. If you are initializing the RS485 interface
  // manually, then the parameter can be empty.
  RS485.begin();

  // Initialize the Modbus RTU client
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
