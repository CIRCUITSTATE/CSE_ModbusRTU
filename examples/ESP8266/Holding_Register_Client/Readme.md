
# CSE_ModbusRTU Arduino Library

## Example - Holding_Register_Client [ESP8266]

Filename: **Holding_Register_Client.ino**

This example demonstrates the use of the `CSE_ModbusRTU` library to read and write to Holding Registers on a Modbus RTU node. This is the client side program for this example. On the server side you can use the [**Holding_Register_Server.ino**](/examples/ESP8266/Holding_Register_Server/Holding_Register_Server.ino) example. We are using a software serial port for the RS-485 interface since the ESP8266 only has one hardware serial port. This example is written for and tested with **NodeMCU-ESP12E** board and the **MAX485+CD4069** RS485 module with automatic data-direction control.

This library depends on the `CSE_ArduinoRS485` library. We need to create an `RS485Classs` port for communicating with the transceivers. In this example, we are using a software serial port called `softSerial` for the RS-485 interface.

```cpp
// Use the software serial port for the RS485 interface and the hardware serial port for
// the debugging output.
SoftwareSerial softSerial (PIN_RS485_RX, PIN_RS485_TX); // RX, TX
```

After creating the serial port, we can pass the new serial port to the `RS485Class` constructor. The following line does not define a DE or RE pins since we are using an RS485 module with auto data-direction control. But if your module needs these pins, add the GPIO pins here.

```cpp
// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, -1, -1, PIN_RS485_TX); // (Serial Port, DE, RE, TX)
```

Next, we will create a new `CSE_ModbusRTU` object called `modbusRTU`. Every Modbus RTU node should have at least one `CSE_ModbusRTU` object associated with it. This is where you can attach Modbus RTU server and client to.

```cpp
// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x02, "modbusRTU-0x02"); // (RS-485 Port, Device Address, Device Name)
```

A new `CSE_ModbusRTU_Client` type client called `modbusRTUClient` is created and added to the previously created `modbusRTU` node. The `name` parameter can be used during debugging.

```cpp
// Create a Modbus RTU client instance with the Modbus RTU node.
CSE_ModbusRTU_Client modbusRTUClient (modbusRTU, "modbusRTUClient"); // (CSE_ModbusRTU, Client Name)
```

In the `setup()` function, the RS485 serial port is initialized manually with custom configuration and pins. If your board does not support changing pins or custom UART configuration, simply replace it with a compatible serial `begin()` call.

```cpp
// Initialize the RS485 port manually.
// This particular begin() call is specific to ESP8266-Arduino.
PORT_RS485.begin (9600, SWSERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);
```

A `CSE_ArduinoRS485` serial port has the ability to initialize the passed UART serial port. But since the serial port here is manually initialized, we can ask the `RS485` port not to initialize the port again by passing no baudrate in the `begin()` call. You can even remove the `begin()` call altogether in that case.

```cpp
// Initialize the RS485 interface. If you are initializing the RS485 interface
// manually, then the parameter can be empty.
RS485.begin();
```

Next, we can initialize the `modbusRTUClient` and add the address of the remote server we want to communicate with. The address is `0x01` in this case.

```cpp
// Initialize the Modbus RTU client.
modbusRTUClient.begin();
// Set the address of the Server you want to communicate with.
// You can change this anytime to communicate with a new Server.
modbusRTUClient.setServerAddress (0x01);
```

If you want the library to print the debug messages, you can enable them here.

```cpp
// Enable/Disable the debug messages here.
  // CSE_ModbusRTU_Debug:: enableDebugMessages();
  CSE_ModbusRTU_Debug:: disableDebugMessages();
```

In the `loop()` function, we will cycle through the following operations:

- Read a single holding register from the server.
- Read multiple holding registers from the server.
- Write a single holding register to the server.
- Read the value back from the server.
- Write multiple holding registers to the server.
- Read the values back from the server.
- Restore old values on the server.

You have to make sure that a server is connected to the client node and it is running. The code for these functions are well-structured and well-commented. So we hope you will be able to understand them easily. If you are using VS Code for writing the code, you can get code suggestions and documentation within the editor. We have added enough documentation for you to understand the code.
