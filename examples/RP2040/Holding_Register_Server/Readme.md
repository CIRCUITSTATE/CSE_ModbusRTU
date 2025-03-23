
# CSE_ModbusRTU Arduino Library

## Example - Holding_Register_Server [RP2040]

Filename: **Holding_Register_Server.ino**

This example demonstrates the use of the `CSE_ModbusRTU` library to read and write to Holding Registers on a Modbus RTU server node. This is the server side program for this example. On the client side you can use the [**Holding_Register_Client.ino**](/examples/RP2040/Holding_Register_Client/Holding_Register_Client.ino) example. We are using hardware serial port for the RS-485 interface. This example is written for and tested with **Raspberry Pi Pico RP2040** board with multiple hardware serial ports and **MAX485+CD4069** RS485 module with automatic data-direction control.

This library depends on the `CSE_ArduinoRS485` library for RS-485. We need to create an `RS485Classs` port for communicating with the transceivers. In this example, a port object `RS485` is created for this, using `Serial1` as the serial interface. You can change this to any compatible Arduino serial ports. If a hardware serial ports like `Serial1` is not available, you can create a `SoftwareSerial` port and pass it the same way. This can be done for boards like Arduino Uno, Nano and other AVR boards. We are using GPIOs `0` and `1` for the serial port and you can also change this if your board supports it (eg. ESP32, RP2040).

The following line creates a new RS-485 port object. We are using both DE and RE pins here. If you do not want to use these pins, you can send `-1` instead.

```cpp
// Declare the RS485 interface here with a hardware serial port.
RS485Class RS485 (PORT_RS485, PIN_RS485_DE, PIN_RS485_RE, PIN_RS485_TX); // Port, DE, RE, TX
```

Next, we will create a new `CSE_ModbusRTU` object called `modbusRTU`. Every Modbus RTU node should have at least one `CSE_ModbusRTU` object associated with it. This is where you can attach Modbus RTU server and client to.

```cpp
// Create a Modbus RTU node instance with the RS485 interface.
CSE_ModbusRTU modbusRTU (&RS485, 0x01, "modbusRTU-0x01"); // (RS-485 Port, Device Address, Device Name)
```

A new `CSE_ModbusRTU_Server` server called `modbusRTUServer` is created and added to the previously created `modbusRTU` node. The `name` parameter can be used during debugging.

```cpp
// Create a Modbus RTU server instance with the Modbus RTU node.
CSE_ModbusRTU_Server modbusRTUServer (modbusRTU, "modbusRTUServer"); // (CSE_ModbusRTU, Server Name)
```

In the `setup()` function, the RS-485 serial port is initialized manually with a custom configuration. If your board does not support changing pins or custom UART configuration, simply replace it with a compatible serial `begin()` call.

```cpp
// Initialize the RS485 port manually.
// This particualr begin() call is specific to RP2040-Arduino.
// If you are using a different controller, change the begin() call accordingly.
PORT_RS485.begin (9600, SERIAL_8N1);
```

A `CSE_ArduinoRS485` serial port has the ability to initialize the passed UART serial port. But since the serial port here is manually initialized, we can ask the `RS485` port not to initialize the port again by passing no baudrate in the `begin()` call. You can even remove the `begin()` call altogether in that case.

```cpp
// Initialize the RS485 interface. If you are initializing the RS485 interface
// manually, then the parameter can be empty.
RS485.begin();
```

Next, we can initialize the `modbusRTUServer`. Since the address of the server is `0x01` in this case, any client that wants to connect to this server should use the address `0x01`.

```cpp
// Initialize the Modbus RTU client.
modbusRTUServer.begin();
```

Next, the following line creates nine Holding Registers starting at address `0x00`.

```cpp
// Configure nine Holding Registers starting at address 0x00.
modbusRTUServer.configureHoldingRegisters (0x00, 9);
```

The values of the first four holding registers are configured next. We are using random statis data for this example. The remaining registers will have the default data.

```cpp
// Set the value of the first four Holding Registers.
modbusRTUServer.writeHoldingRegister (0x00, 0x1234);
modbusRTUServer.writeHoldingRegister (0x01, 0x4321);
modbusRTUServer.writeHoldingRegister (0x02, 0xFF00);
modbusRTUServer.writeHoldingRegister (0x03, 0x00FF);
```

If you want the library to print the debug messages, you can enable them here.

```cpp
// Enable/Disable the debug messages here.
  // CSE_ModbusRTU_Debug:: enableDebugMessages();
  CSE_ModbusRTU_Debug:: disableDebugMessages();
```

In the `loop()` function, we will simply poll the RS-485 line and check for any incoming messages. If an active client node is connected to this server, and requesting the data, the server will respond with the data. Since the responses are automatically handled by the server code inside the library, you do not need to do anything in your application code. You can also update the values in the holding registers when you are not responding to the client requests.

```cpp
// Poll for Modbus RTU requests.
int requestReceived = modbusRTUServer.poll();
delay (10)
```

If you are using VS Code for writing the code, you can get code suggestions and documentation within the editor. We have added enough documentation for you to understand the code.
