
# CSE_ModbusRTU Arduino Library

## Example - ModbusRTU_Client_LED [ESP8266]

Filename: **ModbusRTU_Client_LED.ino**

This example demonstrates the use of a Modbus RTU client to control an LED connected to a remote Modbus RTU server. The client generates periodic requests to the server to turn on and turn off the LED. The request consists of writing a coil located at address `0x0000` at the server. You can use the [**ModbusRTU_Server_LED.ino**](/examples/ESP8266/ModbusRTU_Server_LED/ModbusRTU_Server_LED.ino) example to run the server. We are using a software serial port for the RS-485 interface since the ESP8266 only has one hardware serial port. This example is written for and tested with **NodeMCU-ESP12E** board and the **MAX485+CD4069** RS485 module with automatic data-direction control.

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
// Initialize the Modbus RTU client
modbusRTUClient.begin();
modbusRTUClient.setServerAddress (0x01); // Set the server address to 0x01
```

In the `loop()` function, we can use the `writeCoil()` function of the `CSE_ModbusRTU_Client` class to write to a single coil at the remote server previously specified. You have to make sure the client device is connected to an RS-485 network with a server device with address `0x01`. The server should also have a coil data with address `0x0000`. If the write operation fails for some reason, the function returns `-1`. If the returned value is not `-1`, then we are successful in writing to the coil. The following code will turn the LED on.

```cpp
if (modbusRTUClient.writeCoil (0x00, 0xFF00) == -1) { // Turn the LED on
  Serial.println ("Turning LED on failed.");
}
else {
  Serial.println ("Turning LED on success.");
}
```

Similarly, we can turn the LED off by writing `0x0000` to the same coil after a delay.

```cpp
if (modbusRTUClient.writeCoil (0x00, 0x0000) == -1) { // Turn the LED off
  Serial.println ("Turning LED off failed.");
}
else {
  Serial.println ("Turning LED off success.");
}
```

The effect will be a blinking LED on the server side.
