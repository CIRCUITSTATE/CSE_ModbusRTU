
# CSE_ModbusRTU Arduino Library

## Example - ModbusRTU_Server_LED

Filename: **ModbusRTU_Server_LED.ino**

This example demonstrates the use of a Modbus RTU server to listen for Modbus requests from a client and turn an LED on and off. The request can come from any Modbus RTU client including the `ModbusRTU_Client_LED.ino` example. We define 4 coils but only use the one at `0x00` to control the LED. A client can write to that coil to turn on or turn off the LED. We are using hardware serial port for the RS-485 interface. This example is written for and tested with **DOIT-ESP32-DevKit-V1** board and **MAX485+CD4069** RS485 module with automatic data-direction control.

This library depends on the `CSE_ArduinoRS485` library. We need to create an `RS485Classs` port for communicating with the transceivers. In this example, a port object `RS485` is created for this, using `Serial2` as the serial interface. You can change this to any compatible Arduino serial ports. If a hardware serial ports like `Serial2` is not available, you can create a `SoftwareSerial` port and pass it the same way. We are using GPIOs `16` and `17` for the serial port and you can also change this if your board supports it (eg. ESP32).

The following line does not define a DE or RE pins since we are using an RS485 module with auto data-direction control. But if your module needs these pins, add the GPIO pins here.

```cpp
// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, -1, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)
```

Next, we will create a new `CSE_ModbusRTU` object called `modbusRTU`. Every Modbus RTU should have at least one `CSE_ModbusRTU` object. This is where you can attach Modbus RTU server and client.

```cpp
// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x01, "modbusRTU-0x01"); // (RS-485 Port, Device Address, Device Name)
```

A new `CSE_ModbusRTU_Server` client called `modbusRTUServer` is created and added to the previously created `modbusRTU` node. The name parameter can be used during debugging.

```cpp
// Create a Modbus RTU server instance with the Modbus RTU node.
CSE_ModbusRTU_Server modbusRTUServer (modbusRTU, "modbusRTUServer"); // (CSE_ModbusRTU, Server Name)
```

In the `setup()` function, the RS485 serial port is initialized manually with custom configuration and pins. If your board does not support changing pins or custom UART configuration, simply replace it with a compatible serial `begin()` call.

```cpp
// Initialize the RS485 port manually
PORT_RS485.begin (9600, SERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);
```

A `CSE_ArduinoRS485` serial port has the ability to initialize the passed UART serial port. But since the serial port here is manually initialized, we can ask the `RS485` port not to initialize the port again by passing no baudrate in the `begin()` call. You can even remove the `begin()` call altogether in that case.

```cpp
// Initialize the RS485 interface. If you are initializing the RS485 interface
// manually, then the parameter can be empty.
RS485.begin();
```

In the following lines, we can configure the LED pin as an output and turn it off initially.

```cpp
// Configure the LED
pinMode (ledPin, OUTPUT);
digitalWrite (ledPin, LOW);
```

Next, we can initialize the `modbusRTUServer` and create four new coils with a starting address of `0x00`. All coils will be initialized to `0x0000` by default.

```cpp
// Initialize the Modbus RTU server
modbusRTUServer.begin();
// Configure four coils starting at address 0x00
modbusRTUServer.configureCoils (0x00, 4);
```

In the `loop()` function, we can the `poll()` method of the `CSE_ModbusRTU_Server` class to listen for new requests from a client. Calling the `poll()` method will put the server in listening (receiving) mode until you disable the mode with `RS485.disableReceive()` call. Sending responses will override the receive mode and is therefore not affected even when the receive mode is active.

The `poll()` method will return the function code of a request, if a valid request was received. So we can use that value to run a code block.

```cpp
// Poll for Modbus RTU requests
int requestReceived = modbusRTUServer.poll();
```

In the following code block, we check if a valid request was received. If yes, we can assume that the request came from a client to change the coil value at address `0x00`. To make sure, we can read the coil value from within the server node and check if the value is `1` or `0`. If the value is `1`, we can turn the LED on. If the value is `0`, we can turn the LED off. If the value is neither `1` nor `0`, then we can assume that there was an error reading the coil value.

TODO: What if you have hundreds of coils and other data in the server? How do you know which coil was written to? Currently, there is now way of knowing that, unless you decode the request yourself. This is a limitation of the library and will be fixed in the near future.

```cpp
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
```
