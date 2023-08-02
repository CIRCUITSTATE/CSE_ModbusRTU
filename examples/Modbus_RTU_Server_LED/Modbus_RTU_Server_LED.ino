
//===================================================================================//
/**
 * @file Modbus_RTU_Server_LED.ino
 * @brief Example Arduino sketch for turning an LED on and off using Modbus RTU.
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

const int ledPin = LED_BUILTIN;

// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (Serial1, 2, 3, 4); // DE, RE, TX

// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x01, "modbusRTU-0x01");

// Create a Modbus RTU server instance with the Modbus RTU node.
CSE_ModbusRTU_Server modbusRTUServer (modbusRTU, "modbusRTUServer");

//===================================================================================//

void setup() {
  Serial.begin (9600);
  Serial.println ("Modbus RTU Server LED");

  // Initialize the RS485 interface
  RS485.begin (9600);

  // Initialize the Modbus RTU server
  modbusRTUServer.begin();

  // configure the LED
  pinMode (ledPin, OUTPUT);
  digitalWrite (ledPin, LOW);

  // configure a single coil at address 0x00
  modbusRTUServer.configureCoils (0x00, 4);
}

//===================================================================================//

void loop() {
  // poll for Modbus RTU requests
  int requestReceived = modbusRTUServer.poll();

  if ((requestReceived != -1) && (requestReceived < 0x80)) {
    Serial.println ("Request received");
    // read the current value of the coil
    int coilValue = modbusRTUServer.readCoil (0x00);
  
    if (coilValue == 1) {
      // coil value set, turn LED on
      digitalWrite (ledPin, HIGH);
    }
    else if (coilValue == 0) {
      // coil value clear, turn LED off
      digitalWrite (ledPin, LOW);
    }
    else {
      Serial.println ("Error reading coil");
    }
  }
}

//===================================================================================//
