# CSE_ArduinoRS485 Library API Reference

Version 0.0.6, +05:30 01:06:08 PM 21-10-2023, Saturday

## Index

- [CSE\_ArduinoRS485 Library API Reference](#cse_arduinors485-library-api-reference)
  - [Index](#index)
  - [Classes](#classes)
  - [Class `CSE_ModbusRTU_ADU`](#class-cse_modbusrtu_adu)
    - [`CSE_ModbusRTU_ADU()`](#cse_modbusrtu_adu)
    - [`resetLength()`](#resetlength)
    - [`getLength()`](#getlength)
    - [`clear()`](#clear)
    - [`add()`](#add)
    - [`checkCRC()`](#checkcrc)
    - [`calculateCRC()`](#calculatecrc)
    - [`setType()`](#settype)
    - [`setDeviceAddress()`](#setdeviceaddress)
    - [`setFunctionCode()`](#setfunctioncode)
    - [`setExceptionCode()`](#setexceptioncode)
    - [`setException()`](#setexception)
    - [`setData()`](#setdata)
    - [`setCRC()`](#setcrc)
    - [`getDeviceAddress()`](#getdeviceaddress)
    - [`getFunctionCode()`](#getfunctioncode)
    - [`getExceptionCode()`](#getexceptioncode)
    - [`getStartingAddress()`](#getstartingaddress)
    - [`getQuantity()`](#getquantity)
    - [`getCRC()`](#getcrc)
    - [`getDataLength()`](#getdatalength)
    - [`getByte()`](#getbyte)
    - [`getWord()`](#getword)
    - [`getType()`](#gettype)
    - [`print()`](#print)
  - [Class `CSE_ModbusRTU`](#class-cse_modbusrtu)
    - [`CSE_ModbusRTU()`](#cse_modbusrtu)
    - [`getName()`](#getname)
    - [`setServer()`](#setserver)
    - [`setClient()`](#setclient)
    - [`receive()`](#receive)
    - [`send()`](#send)
  - [Class `CSE_ModbusRTU_Server`](#class-cse_modbusrtu_server)
    - [`CSE_ModbusRTU_Server()`](#cse_modbusrtu_server)
    - [`getName()`](#getname-1)
    - [`begin()`](#begin)
    - [`poll()`](#poll)
    - [`receive()`](#receive-1)
    - [`send()`](#send-1)
    - [`configureCoils()`](#configurecoils)
    - [`configureDiscreteInputs()`](#configurediscreteinputs)
    - [`configureInputRegisters()`](#configureinputregisters)
    - [`configureHoldingRegisters()`](#configureholdingregisters)
    - [`readCoil()`](#readcoil)
    - [`writeCoil()`](#writecoil)
    - [`isCoilPresent()`](#iscoilpresent)
    - [`readDiscreteInput()`](#readdiscreteinput)
    - [`writeDiscreteInput()`](#writediscreteinput)
    - [`isDiscreteInputPresent()`](#isdiscreteinputpresent)
    - [`readInputRegister()`](#readinputregister)
    - [`writeInputRegister()`](#writeinputregister)
    - [`isInputRegisterPresent()`](#isinputregisterpresent)
    - [`readHoldingRegister()`](#readholdingregister)
    - [`writeHoldingRegister()`](#writeholdingregister)
    - [`isHoldingRegisterPresent()`](#isholdingregisterpresent)
  - [Class `CSE_ModbusRTU_Client`](#class-cse_modbusrtu_client)
    - [`CSE_ModbusRTU_Client()`](#cse_modbusrtu_client)
    - [`setServerAddress()`](#setserveraddress)
    - [`getName()`](#getname-2)
    - [`begin()`](#begin-1)
    - [`receive()`](#receive-2)
    - [`send()`](#send-2)
    - [`readCoil()`](#readcoil-1)
    - [`writeCoil()`](#writecoil-1)
    - [`readDiscreteInput()`](#readdiscreteinput-1)
    - [`readInputRegister()`](#readinputregister-1)
    - [`readHoldingRegister()`](#readholdingregister-1)
    - [`writeHoldingRegister()`](#writeholdingregister-1)



## Classes

* `CSE_ModbusRTU_ADU` - Generic class to store Modbus RTU Application Data Unit (ADU).
* `CSE_ModbusRTU` - Generic Modbus RTU class. Implements common functions and data structures needed for both Modbus RTU server and client.
* `CSE_ModbusRTU_Server` - Implements the Modbus RTU server node.
* `CSE_ModbusRTU_Client` - Implements the Modbus RTU client node.
* `modbus_bit_t` - Modbus bit data type. Can be used for coils and discrete inputs.
* `modbus_register_t` - Modbus register data type. Can be used for holding registers and input registers.

## Class `CSE_ModbusRTU_ADU`

### `CSE_ModbusRTU_ADU()`

This constructor creates a new `CSE_ModbusRTU_ADU` object. The ADU buffer is cleared during instantiation.

#### Syntax

```cpp
CSE_ModbusRTU_ADU adu;
```

##### Parameters

None

##### Returns

`CSE_ModbusRTU_ADU` object

### `resetLength()`

Resets the `aduLength` to `0` without clearing any data on the buffer. This is useful for a faster write to the buffer without waiting for the buffer to be cleared. Resetting the `aduLength` will prevent you from doing any operation on the buffer even if it has valid data. So use this function with caution, and only before you start writing to the buffer.

#### Syntax

```cpp
adu.resetLength();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` if the length was reset successfully.
  * `false` otherwise.

### `getLength()`

Returns the number of valid bytes in the ADU buffer. A value `0` indicates that the buffer has no valid data even though its contents may not be all `0x00`. The `aduLength` is a very important variable. It is used to keep track of the number of valid bytes in ADU buffer. If you change the `aduLength` manually, you will end up with a corrupted ADU buffer.

#### Syntax

```cpp
adu.getLength();
```

##### Parameters

None

##### Returns

* _`uint8_t`_ : The current `aduLength`.

### `clear()`

Clear the ADU buffer.

#### Syntax 1

Clear the ADU buffer by setting all bytes to `0x00`.

```cpp
adu.clear();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` if the buffer was cleared successfully.
  * `false` otherwise.

#### Syntax 2

Clear a single byte in the ADU buffer by setting it to `0x00`.

```cpp
adu.clear (uint8_t index);
```

##### Parameters

* `index` : The index of the byte to be cleared.

##### Returns

* _`bool`_ :
  * `true` if the byte was cleared successfully.
  * `false` otherwise.

### `add()`

#### Syntax 1

Add a single byte to the ADU buffer. The new byte is written at the end of the buffer indicated by `aduLength`. The `aduLength` is incremented by `1`.

```cpp
adu.add (uint8_t byte);
```

##### Parameters

* `byte` : The byte to be added to the buffer.

##### Returns

* _`bool`_ :
  * `true` if the byte was added successfully.
  * `false` otherwise.

#### Syntax 2

Add a byte buffer to the ADU buffer. The new bytes are written at the end of the buffer indicated by `aduLength`. The `aduLength` is incremented by the number of bytes added.

```cpp
adu.add (uint8_t *buffer, uint8_t length);
```

##### Parameters

* `buffer` : The buffer containing the bytes to be added.
* `length` : The number of bytes to be added.

##### Returns

* _`bool`_ :
  * `true` if the bytes were added successfully.
  * `false` otherwise.

#### Syntax 3

Add a word (16-bit) to the ADU buffer. The new word is written to the end of the buffer indicated by `aduLength`. The `aduLength` is incremented by `2`.

```cpp
adu.add (uint16_t word);
```

##### Parameters

* `word` : The word to be added to the buffer.

##### Returns

* _`bool`_ :
  * `true` if the word was added successfully.
  * `false` otherwise.

#### Syntax 4

Add a word array to the ADU buffer. The new words are written to the end of the buffer indicated by `aduLength`. The `aduLength` is incremented by the length of the word array times `2`.

```cpp
adu.add (uint16_t* buffer, uint8_t length);
```

##### Parameters

* `buffer` : The word array to be added to the buffer.
* `length` : The number of words to be added.

##### Returns

* _`bool`_ :
  * `true` if the words were added successfully.
  * `false` otherwise.

### `checkCRC()`

Calculates the CRC of the ADU and compares it to the CRC in the ADU. If the ADU length is less than `3`, that means that the device address, function code, and data are not set yet. In this case, we can't calculate the CRC and the function returns `false`.

#### Syntax

```cpp
adu.checkCRC();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` if the CRC matches.
  * `false` otherwise.

### `calculateCRC()`

Calculates the CRC of the ADU contents and returns it. If the `aduLength` is not sufficient to calculate the CRC, then `0x00` is returned. The CRC can be calculated for the entire ADU buffer, or exclude the last two bytes if the CRC is already set.

#### Syntax

```cpp
adu.calculateCRC (bool isCRCSet);
```

##### Parameters

* `isCRCSet` : Optional.
  * Default: `false`.
  * `true` : Calculate the CRC for the entire ADU buffer.
  * `false` : Calculate the CRC for the entire ADU buffer except the last two bytes.

##### Returns

* _`uint16_t`_ : The calculated CRC.

### `setType()`

Set the type of the ADU. Setting the type does not check if the data in the ADU is valid. For example, if you are setting the type to `EXCEPTION`, this function doesn't check if the function code is valid for an exception. So you should only use this function after you have set the ADU data correctly.

#### Syntax

```cpp
adu.setType (int type);
```

##### Parameters

* `type` : The type of the ADU.
  * `aduType_t:: NONE` : No type.
  * `aduType_t:: REQUEST` : Request.
  * `aduType_t:: RESPONSE` : Response.
  * `aduType_t:: EXCEPTION` : Exception.

##### Returns

* _`bool`_ :
  * `true` if the type was set successfully.
  * `false` otherwise.

### `setDeviceAddress()`

Set the device address field on the ADU. The device address can be client or server address depending on where it is used. Ideally, you should set the device address first before setting other fields. The ADU length will be incremented only if the current length is less than `2`. If you set the device address after setting other fields, the ADU length will not be incremented.

#### Syntax

```cpp
adu.setDeviceAddress (uint8_t address);
```

##### Parameters

* `address` : Client or server address (8-bit).

##### Returns

* _`bool`_ :
  * `true` if the device address was set successfully.
  * `false` otherwise.

### `setFunctionCode()`

Set the function code field on the ADU. The value of the function code should be from `0x01` to `0x7F`. The `aduLength` is incremented only if the current length is less than `2`. Ideally, you should set the function code after setting the device address.

#### Syntax

```cpp
adu.setFunctionCode (uint8_t functionCode);
```

##### Parameters

* `functionCode` : Valid Modbus function code from `0x01` to `0x7F` (1 - 127).

##### Returns

* _`bool`_ :
  * `true` if the function code was set successfully.
  * `false` otherwise.

### `setExceptionCode()`

Set the exception code just after the function code. The `aduLength` should be exactly `2` for this to work. So you must set the device address and function code first before setting the exception code. The `aduLength` is incremented by `1`.

#### Syntax

```cpp
adu.setExceptionCode (uint8_t exceptionCode);
```

##### Parameters

* `exceptionCode` : Valid Modbus exception code from `0x01` to `0x7F` (1 - 127).

##### Returns

* _`bool`_ :
  * `true` if the exception code was set successfully.
  * `false` otherwise.

### `setException()`

Convert the function code to an exception by setting the MSB to `1`. Will not work if the function code is already an exception (>= `0x80`). `aduLength` is not changed.

#### Syntax

```cpp
adu.setException();
```

##### Parameters

None

##### Returns

* _`bool`_ :
  * `true` if the function code was converted to an exception successfully.
  * `false` otherwise.

### `setData()`

Set the data field of the ADU. The data will be set right after the end of the function code field. You must set the device address and function code first. Otherwise the operation fails. You are free to change the data later after setting it successfully once. This will rewrite existing data. The data length should not exceed the maximum limit. Otherwise the operation fails.

You should also calculate and set the CRC right after writing the data.

#### Syntax

```cpp
adu.setData (uint8_t* buffer, uint8_t length);
```

##### Parameters

* `buffer` : A pointer to the data buffer.
* `length` : The length of the data buffer.

##### Returns

* _`bool`_ :
  * `true` if the data was set successfully.
  * `false` otherwise.

### `setCRC()`

Calculate the CRC based on the data in the ADU, and write it at the end of the ADU. You must set valid device address, function code and data before setting the CRC field. If the ADU length is less than `3`, the operation fails and returns `0x00`, which is not a valid CRC. The calculated CRC is also returned. You must check the return value if you want to be sure the CRC is set correctly.

#### Syntax

```cpp
adu.setCRC();
```

##### Parameters

None

##### Returns

* _`uint16_t`_ : The calculated CRC.

### `getDeviceAddress()`

Returns the device address field data from the ADU. The address can be client or server address depending on what you are using the ADU for.

#### Syntax

```cpp
adu.getDeviceAddress();
```

##### Parameters

None

##### Returns

* _`uint8_t`_ : The device address byte in the ADU.

### `getFunctionCode()`

Returns the function code from the ADU.

#### Syntax

```cpp
adu.getFunctionCode();
```

##### Parameters

None

##### Returns

* _`uint8_t`_ : The function code byte in the ADU.

### `getExceptionCode()`

Returns the exception code from the ADU. In a valid exception ADU, the code comes just after the function code. Only works for ADU  with the type set to `EXCEPTION` or the exception bit (MSB) in the function code is set. So make sure you check the ADU type before calling this function.

#### Syntax

```cpp
adu.getExceptionCode();
```

##### Parameters

None

##### Returns

* _`uint8_t`_ : The exception code byte in the ADU.

### `getStartingAddress()`

Returns the starting address field in the ADU. The function only reads the value at the position, and does not guarantee that the value is valid.

#### Syntax

```cpp
adu.getStartingAddress();
```

##### Parameters

None

##### Returns

* _`uint16_t`_ : The starting address word in the ADU.

### `getQuantity()`

Returns the quantity field in the ADU. The quantity is the register count in most cases. The function only reads the value at the position, and does not guarantee that the value is valid.

#### Syntax

```cpp
adu.getQuantity();
```

##### Parameters

None

##### Returns

* _`uint16_t`_ : The quantity word in the ADU.

### `getCRC()`

Returns the CRC field from the ADU. Operation fails if the ADU length is less than `4`.

#### Syntax

```cpp
adu.getCRC();
```

##### Parameters

None

##### Returns

* _`uint16_t`_ : The CRC word in the ADU.

### `getDataLength()`

Returns the length of data field in the ADU. This is the number of bytes after the function code and before the CRC. The length is calculated purely from the `aduLength`. So if the `aduLength` is not correct for some reason, the function will return an incorrect value. The ADU length should be at least `5` bytes.

#### Syntax

```cpp
adu.getDataLength();
```

##### Parameters

None

##### Returns

* _`uint8_t`_ : The length of the data field in the ADU.

### `getByte()`

Returns a single byte from the ADU. The index should be within the `aduLength`. If the index is not valid (greater than `aduLength`), the function returns `0x00`. So this does not guarantee that the function will always return a valid byte.

#### Syntax

```cpp
adu.getByte (uint8_t index);
```

##### Parameters

* `index` : The index of the byte to be returned.

##### Returns

* _`uint8_t`_ : The byte at the specified index.

### `getWord()`

Returns the 16-bit word at the specified index. The index should be within the `aduLength`. If the index is not valid (greater than `aduLength`), the function returns `0x0000`. So this does not guarantee that the function will always return a valid word. Two consecutive bytes are combined to form the word. The index indicates the position of the `Hi` byte. The operation fails if the ADU length is less than `2`.

#### Syntax

```cpp
adu.getWord (uint8_t index);
```

##### Parameters

* `index` : The index of the `Hi` byte of the word to be returned.

##### Returns

* _`uint16_t`_ : The word at the specified index.

### `getType()`

Returns the current type of the ADU. Valid type can be any `aduType_t`. The ADU type is converted to an integer.

#### Syntax

```cpp
adu.getType();
```

##### Parameters

None

##### Returns

* _`int`_ : The current type of the ADU. Converted from `aduType_t`.

### `print()`

Prints the ADU as a string of hex numbers to the default debug serial port (`MODBUS_DEBUG_SERIAL`).

#### Syntax

```cpp
adu.print();
```

##### Parameters

None

##### Returns

None

## Class `CSE_ModbusRTU`

A generic Modbus RTU protocol class. It implements common functions and data structures needed for both Modbus RTU server and client nodes. Note that this does not create an actual client or server device. Use `CSE_ModbusRTU_Client` or `CSE_ModbusRTU_Server` creating device objects and then attach them to a `CSE_ModbusRTU` object. You can attach one client and one server to a single `CSE_ModbusRTU` object at a time. Which role to be used is completely up to you.

The 'send()` and `receive()` functions are shared between the server and client devices attached to the same `CSE_ModbusRTU` object. So only device should access the serial port at a time. Please be aware of this if you are running a server and client in different threads.

TODO: Add parallel access protection.

### `CSE_ModbusRTU()`

Instantiate a new `CSE_ModbusRTU` object. The device address is the host device address. Serial port can be hardware or software serial port if you are using the `CSE_ArduinoRS485` library.

#### Syntax

```cpp
CSE_ModbusRTU (serialPort_t serialPort, uint8_t deviceAddress, String name);
```

##### Parameters

* `serialPort` : The serial port to be used for Modbus communication.
* `deviceAddress` : The device address of the host device.
* `name` : The name of the Modbus node.

##### Returns

`CSE_ModbusRTU` object. An example object `node` will be used in this documentation.

### `getName()`

Returns the name of the Modbus RTU object.

#### Syntax

```cpp
node.getName();
```

##### Parameters

None

##### Returns

* _`String`_ : The name of the Modbus RTU object.

### `setServer()`

This allows you to add a new `CSE_ModbusRTU_Server` object to the `CSE_ModbusRTU` object. A server will be able to respond to Modbus RTU requests from a client. You can have only one server and client per `CSE_ModbusRTU` object.

#### Syntax

```cpp
node.setServer (CSE_ModbusRTU_Server& server);
```

##### Parameters

* `server` : A reference to the `CSE_ModbusRTU_Server` object to be added.

##### Returns

* _`bool`_ :
  * `true` if the server was added successfully.
  * `false` otherwise.

### `setClient()`

This allows you to add a new `CSE_ModbusRTU_Client` to the `CSE_ModbusRTU` object. A client will be able to send Modbus RTU requests to servers. You can have only one server and client per `CSE_ModbusRTU` object.

#### Syntax

```cpp
node.setClient (CSE_ModbusRTU_Client& client);
```

##### Parameters

* `client` : A reference to the `CSE_ModbusRTU_Client` object to be added.

##### Returns

* _`bool`_ :
  * `true` if the client was added successfully.
  * `false` otherwise.

### `receive()`

Reads the serial port and save an incoming ADU to the specified ADU object. The `aduLength` is reset to `0` before reading the serial port. The function will check the CRC of the received ADU and return the length of the ADU if the CRC is valid. If the ADU is not valid, `-1` is returned. The address of the ADU is not checked. It has to be checked by the server or client.

#### Syntax

```cpp
node.receive (CSE_ModbusRTU_ADU& adu, uint32_t timeout);
```

##### Parameters

* `adu` : A reference to the `CSE_ModbusRTU_ADU` object to store the received ADU.
* `timeout` : Optional.
  * Default: `100`.
  * The timeout in milliseconds to wait for the ADU. If the timeout is reached, the function returns `0`.

##### Returns

* _`int`_ : The length of the received ADU. `-1` if the operation fails.

### `send()`

Sends the specified ADU to the serial port. The function will check the CRC before sending it. Returns the ADU length if the operation was successful; `-1` otherwise.

#### Syntax

```cpp
node.send (CSE_ModbusRTU_ADU& adu);
```

##### Parameters

* `adu` : A reference to the `CSE_ModbusRTU_ADU` object to be sent.

##### Returns

* _`int`_ : The length of the ADU sent. `-1` if the operation fails.

## Class `CSE_ModbusRTU_Server`

Implements the Modbus RTU server node. A server can respond to Modbus RTU requests from a client. You can have only one server and client per `CSE_ModbusRTU` object. The `send()` and `receive()` functions are shared between the server and client devices attached to the same `CSE_ModbusRTU` object. So only device should access the serial port at a time. Please be aware of this if you are running a server and client in different threads.

### `CSE_ModbusRTU_Server()`

Instantiates a new `CSE_ModbusRTU_Server` object. You must send a parent `CSE_ModbusRTU` object and a name for the server. The name is used for debugging purposes.

#### Syntax

```cpp
CSE_ModbusRTU_Server (CSE_ModbusRTU& rtu, String name);
```

##### Parameters

* `rtu` : A reference to the parent `CSE_ModbusRTU` object.
* `name` : The name of the server.

##### Returns

`CSE_ModbusRTU_Server` object. An example object `server` will be used in this documentation.

### `getName()`

Returns the name of the server.

#### Syntax

```cpp
server.getName();
```

##### Parameters

None

##### Returns

* _`String`_ : The name of the server.

### `begin()`

Does nothing for now.

#### Syntax

```cpp
server.begin();
```

##### Parameters

None

##### Returns

None

### `poll()`

This function is used to poll the serial port for new requests. When a request is received, it is disassembled into the `request` ADU and checked for validity. This function takes care of checking what type of request it is and then send a response back to the client. The response is assembled into the `response` ADU. Finally, the type of request received is returned.

#### Syntax

```cpp
server.poll();
```

##### Parameters

None

##### Returns

* _`int`_ : The function code received from the server. `-1` if the operation fails.

### `receive()`

Receives a request from the client and save it to `request` ADU. This function uses the `receive()` function of the parent `CSE_ModbusRTU` object. The timeout is determined by what is defined in the `CSE_ModbusRTU`.

#### Syntax

```cpp
server.receive();
```

##### Parameters

None

##### Returns

The return value comes from the `receive()` function of the parent `CSE_ModbusRTU` object.

* _`int`_ : The length of the received ADU. `-1` if the operation fails.

### `send()`

Sends a response to the client. The response is assembled into the `response` ADU. This function uses the `send()` function of the parent `CSE_ModbusRTU` object.

#### Syntax

```cpp
server.send();
```

##### Parameters

None

##### Returns

The return value comes from the `send()` function of the parent `CSE_ModbusRTU` object.

* _`int`_ : The length of the ADU sent. `-1` if the operation fails.

### `configureCoils()`

Configures the coil data array by adding new data to the `coils` vector. The maximum coil count is limited to `MODBUS_RTU_COIL_COUNT_MAX` which you can change if required. You can create a contiguous set of coils by specifying the starting address and the quantity of coils to be created. If you want coils of different and non-contiguous addresses, you can call this function multiple times. This library treats individual data as unique and independent; not as part of a contiguous set. So two adjacent coils in the data array can have non-contiguous addresses.

#### Syntax

```cpp
server.configureCoils (uint16_t startAddress, uint16_t quantity);
```

##### Parameters

* `startAddress` : The starting address of the coils.
* `quantity` : The number of coils to be created.

##### Returns

* _`bool`_ :
  * `true` if the coils were configured successfully.
  * `false` otherwise.

### `configureDiscreteInputs()`

Configures the discrete input data array by adding new data to the `discreteInputs` vector. The maximum discrete input count is limited to `MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX` which you can change if required. You can create a contiguous set of discrete inputs by specifying the starting address and the quantity of discrete inputs to be created. If you want discrete inputs of different and non-contiguous addresses, you can call this function multiple times. This library treats individual data as unique and independent; not as part of a contiguous set. So two adjacent discrete inputs in the data array can have non-contiguous addresses.

#### Syntax

```cpp
server.configureDiscreteInputs (uint16_t startAddress, uint16_t quantity);
```

##### Parameters

* `startAddress` : The starting address of the discrete inputs.
* `quantity` : The number of discrete inputs to be created.

##### Returns

* _`bool`_ :
  * `true` if the discrete inputs were configured successfully.
  * `false` otherwise.

### `configureInputRegisters()`

Configures the input register data array by adding new data to the `inputRegisters` vector. The maximum input register count is limited to `MODBUS_RTU_INPUT_REGISTER_COUNT_MAX` which you can change if required. You can create a contiguous set of input registers by specifying the starting address and the quantity of input registers to be created. If you want input registers of different and non-contiguous addresses, you can call this function multiple times. This library treats individual data as unique and independent; not as part of a contiguous set. So two adjacent input registers in the data array can have non-contiguous addresses.

#### Syntax

```cpp
server.configureInputRegisters (uint16_t startAddress, uint16_t quantity);
```

##### Parameters

* `startAddress` : The starting address of the input registers.
* `quantity` : The number of input registers to be created.

##### Returns

* _`bool`_ :
  * `true` if the input registers were configured successfully.
  * `false` otherwise.

### `configureHoldingRegisters()`

Configures the holding register data array by adding new data to the `holdingRegisters` vector. The maximum holding register count is limited to `MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX` which you can change if required. You can create a contiguous set of holding registers by specifying the starting address and the quantity of holding registers to be created. If you want holding registers of different and non-contiguous addresses, you can call this function multiple times. This library treats individual data as unique and independent; not as part of a contiguous set. So two adjacent holding registers in the data array can have non-contiguous addresses.

#### Syntax

```cpp
server.configureHoldingRegisters (uint16_t startAddress, uint16_t quantity);
```

##### Parameters

* `startAddress` : The starting address of the holding registers.
* `quantity` : The number of holding registers to be created.

##### Returns

* _`bool`_ :
  * `true` if the holding registers were configured successfully.
  * `false` otherwise.

### `readCoil()`

Reads a single coil from the `coils` data array. The address is checked for validity by comparing it with the coil addresses in the `coils` data array. If the address is valid, the coil value is returned. If the address is invalid, `-1` is returned.

This function is provided so that a server can read its own local data. This operation does not generate a request/response. For example, you can use this function to read a coil after a request is received from a client to change the value of a coil in the server.

#### Syntax

```cpp
server.readCoil (uint16_t address);
```

##### Parameters

* `address` : The address of the coil to be read.

##### Returns

* _`int`_ : The value of the coil. `-1` if the operation fails.

### `writeCoil()`

Writes to the coil data in the `coils` data array in the server. The address is checked for validity by comparing it with the coil addresses in the `coils` data array. If the address is valid, the coil value is updated and `1` is returned. If the address is invalid, `-1` is returned. The coil value can be `0x00` or `0x01`. Any other value is invalid.

This function is provided so that a server can write its own local data. This operation does not generate a request/response. For example, you can use this function to write a coil after a request is received from a client to change the value of a coil in the server.

#### Syntax 1

Writes a single coil data.

```cpp
server.writeCoil (uint16_t address, uint8_t value);
```

##### Parameters

* `address` : The address of the coil to be written.

##### Returns

* _`int`_ :
  * `1` if the coil was written successfully.
  * `-1` if the operation fails.

#### Syntax 2

Writes multiple coil data. Individual addresses are checked for validity by comparing them with the coil addresses in the `coils` data array. If any of the addresses are invalid, the function stops writing the values and returns  -1.

```cpp
server.writeCoil (uint16_t address, uint8_t value, uint16_t count);
```

##### Parameters

* `address` : The address of the coil to be written.
* `value` : The value of the coil to be written.
* `count` : The number of coils to be written. All coils are written with the same value.

##### Returns

* _`int`_ :
  * `1` if the coils were written successfully.
  * `-1` if the operation fails.

### `isCoilPresent()`

Checks if one or more coils with address is present in the server. The coil addresses do not have to be contiguous. But ideally it should be.

#### Syntax 1

Checks a single coil.

```cpp
server.isCoilPresent (uint16_t address);
```

##### Parameters

* `address` : The address of the coil to be checked.

##### Returns

* _`bool`_ :
  * `true` if the coil is present.
  * `false` otherwise.


#### Syntax 2

Checks multiple coils.

```cpp
server.isCoilPresent (uint16_t address, uint16_t count);
```

##### Parameters

* `address` : The start address of the coils to be checked.
* `count` : The number of coils to be checked.

##### Returns

* _`bool`_ :
  * `true` if all coils are present.
  * `false` otherwise.

### `readDiscreteInput()`

Reads a single discrete input register on the server. The address is checked for validity. If the address is valid, the input register value is returned. If the address is invalid, `-1` is returned.

This function is provided so that a server can read its own local data. This operation does not generate a request/response.

#### Syntax

```cpp
server.readDiscreteInput (uint16_t address);
```

##### Parameters

* `address` : The address of the discrete input to be read.

##### Returns

* _`int`_ : The value of the discrete input.
  * `0x00` : `OFF`
  * `0x01` : `ON`
  * `-1` if the operation fails.

### `writeDiscreteInput()`

Writes to one or more discrete inputs on the server. Even though discrete inputs are read-only, a server can write its own discrete inputs to update their states.

#### Syntax 1

Writes a single discrete input.

```cpp
server.writeDiscreteInput (uint16_t address, uint8_t value);
```

##### Parameters

* `address` : The address of the discrete input to be written.
* `value` : The value of the discrete input to be written.

##### Returns

* _`int`_ :
  * `1` if the discrete input was written successfully.
  * `-1` if the operation fails.

#### Syntax 2

Writes a contiguous series of discrete input registers in the server. Individual addresses are checked for validity by comparing them with the discrete input addresses in the `discreteInputs` data array. If any of the addresses are invalid, the function stops writing the values and returns `-1`. If all the addresses are valid, the discrete input values are updated and `1` is returned.

```cpp
server.writeDiscreteInput (uint16_t address, uint8_t value, uint16_t count);
```

##### Parameters

* `address` : The address of the discrete input to be written.
* `value` : The value of the discrete input to be written.
* `count` : The number of discrete inputs to be written. All discrete inputs are written with the same value.

##### Returns

* _`int`_ :
  * `1` if the discrete inputs were written successfully.
  * `-1` if the operation fails.

### `isDiscreteInputPresent()`

Checks if one or more discrete input with address is present in the server.

#### Syntax 1

Checks a single discrete input.

```cpp
server.isDiscreteInputPresent (uint16_t address);
```

##### Parameters

* `address` : The address of the discrete input to be checked.

##### Returns

* _`bool`_ :
  * `true` if the discrete input is present.
  * `false` otherwise.

#### Syntax 2

Checks if a range of discrete inputs is present. If any of the discrete inputs is not present in the server, the function returns `false`.

```cpp
server.isDiscreteInputPresent (uint16_t address, uint16_t count);
```

##### Parameters

* `address` : The start address of the discrete inputs to be checked.
* `count` : The number of discrete inputs to be checked.

##### Returns

* _`bool`_ :
  * `true` if all discrete inputs are present.
  * `false` otherwise.

### `readInputRegister()`

Reads a single input register in the server. The address is checked for validity. If the address is valid, the input register value is returned. If the address is invalid, `-1` is returned.

This function is provided so that a server can read its own local data. This operation does not generate a request/response.

#### Syntax

```cpp
server.readInputRegister (uint16_t address);
```

##### Parameters

* `address` : The address of the input register to be read.

##### Returns

* _`int`_ :
  * `0x0000` to `0xFFFF` : The value of the input register.
  * `-1` if the operation fails.

### `writeInputRegister()`

Writes to one or more input registers on the server. Even though input registers are read-only, a server can write its own input registers to update their states.

#### Syntax 1

Writes a single input register.

```cpp
server.writeInputRegister (uint16_t address, uint16_t value);
```

##### Parameters

* `address` : The address of the input register to be written.
* `value` : The value of the input register to be written.

##### Returns

* _`int`_ :
  * `1` if the input register was written successfully.
  * `-1` if the operation fails.

#### Syntax 2

Writes a contiguous series of input registers in the server. Individual addresses are checked for validity by comparing them with the input register addresses in the `inputRegisters` data array. If any of the addresses are invalid, the function stops writing the values and returns `-1`. If all the addresses are valid, the input register values are updated and `1` is returned.

```cpp
server.writeInputRegister (uint16_t address, uint16_t value, uint16_t count);
```

##### Parameters

* `address` : The address of the input register to be written.
* `value` : The value of the input register to be written.
* `count` : The number of input registers to be written. All input registers are written with the same value.

##### Returns

* _`int`_ :
  * `1` if the input registers were written successfully.
  * `-1` if the operation fails.

### `isInputRegisterPresent()`

Checks if one or more input registers with address is present in the server.

#### Syntax 1

Checks a single input register.

```cpp
server.isInputRegisterPresent (uint16_t address);
```

##### Parameters

* `address` : The address of the input register to be checked.

##### Returns

* _`bool`_ :
  * `true` if the input register is present.
  * `false` otherwise.

#### Syntax 2

Checks if a range of input registers is present. If any of the input registers is not present in the server, the function returns `false`.

```cpp
server.isInputRegisterPresent (uint16_t address, uint16_t count);
```

##### Parameters

* `address` : The start address of the input registers to be checked.
* `count` : The number of input registers to be checked.

##### Returns

* _`bool`_ :
  * `true` if all input registers are present.
  * `false` otherwise.

### `readHoldingRegister()`

Reads a single holding register in the server. The address is checked for validity. If the address is valid, the holding register value is returned. If the address is invalid, `-1` is returned.

This function is provided so that a server can read its own local data. This operation does not generate a request/response.

#### Syntax

```cpp
server.readHoldingRegister (uint16_t address);
```

##### Parameters

* `address` : The address of the holding register to be read.

##### Returns

* _`int`_ :
  * `0x0000` to `0xFFFF` : The value of the holding register.
  * `-1` if the operation fails.

### `writeHoldingRegister()`

Writes to one or more holding registers on the server. This function is provided so that a server can write its own holding registers to update their states.

#### Syntax 1

Writes a single holding register.

```cpp
server.writeHoldingRegister (uint16_t address, uint16_t value);
```

##### Parameters

* `address` : The address of the holding register to be written.
* `value` : The value of the holding register to be written.

##### Returns

* _`int`_ :
  * `1` if the holding register was written successfully.
  * `-1` if the operation fails.

#### Syntax 2

Writes a contiguous series of holding registers in the server. Individual addresses are checked for validity by comparing them with the holding register addresses in the `holdingRegisters` data array. If any of the addresses are invalid, the function stops writing the values and returns `-1`. If all the addresses are valid, the holding register values are updated and `1` is returned.

```cpp
server.writeHoldingRegister (uint16_t address, uint16_t value, uint16_t count);
```

##### Parameters

* `address` : The address of the holding register to be written.
* `value` : The value of the holding register to be written.
* `count` : The number of holding registers to be written. All holding registers are written with the same value.

##### Returns

* _`int`_ :
  * `1` if the holding registers were written successfully.
  * `-1` if the operation fails.

### `isHoldingRegisterPresent()`

Checks if one or more holding registers with address is present in the server.

#### Syntax 1

Checks a single holding register.

```cpp
server.isHoldingRegisterPresent (uint16_t address);
```

##### Parameters

* `address` : The address of the holding register to be checked.

##### Returns

* _`bool`_ :
  * `true` if the holding register is present.
  * `false` otherwise.

#### Syntax 2

Checks if a range of holding registers is present. If any of the holding registers is not present in the server, the function returns `false`.

```cpp
server.isHoldingRegisterPresent (uint16_t address, uint16_t count);
```

##### Parameters

* `address` : The start address of the holding registers to be checked.
* `count` : The number of holding registers to be checked.

##### Returns

* _`bool`_ :
  * `true` if all holding registers are present.
  * `false` otherwise.

## Class `CSE_ModbusRTU_Client`

Implements the Modbus RTU client node. A client can send Modbus RTU requests to servers. You can have only one server and client per `CSE_ModbusRTU` object. The 'send()` and `receive()` functions are shared between the server and client devices attached to the same `CSE_ModbusRTU` object. So only device should access the serial port at a time. Please be aware of this if you are running a server and client in different threads.

### `CSE_ModbusRTU_Client()`

Instantiates a new `CSE_ModbusRTU_Client` object. You must send a parent `CSE_ModbusRTU` object and a name for the client. The name is used for debugging purposes.

#### Syntax

```cpp
CSE_ModbusRTU_Client (CSE_ModbusRTU& rtu, String name);
```

##### Parameters

* `rtu` : A reference to the parent `CSE_ModbusRTU` object.
* `name` : The name of the client.

##### Returns

`CSE_ModbusRTU_Client` object. An example object `client` will be used in this documentation.

### `setServerAddress()`

Sets the address of the Modbus server you want to communicate with. This sets the `remoteDeviceAddress`. You must call this function before calling any of the read or write functions.

#### Syntax

```cpp
client.setServerAddress (uint8_t remoteAddress);
```

##### Parameters

* `remoteAddress` : The address of the Modbus server.

##### Returns

* _`bool`_ :
  * `true` if the address was set successfully.
  * `false` otherwise.

### `getName()`

Returns the name of the client.

#### Syntax

```cpp
client.getName();
```

##### Parameters

None

##### Returns

* _`String`_ : The name of the client.

### `begin()`

Does nothing for now.

#### Syntax

```cpp
client.begin();
```

##### Parameters

None

##### Returns

None

### `receive()`

Receives a response from the server and save it to `response` ADU. This function uses the `receive()` function of the parent `CSE_ModbusRTU` object. The timeout is determined by `receiveTimeout`.


#### Syntax

```cpp
client.receive();
```

##### Parameters

None

##### Returns

The return value comes from the `receive()` function of the parent `CSE_ModbusRTU` object.

* _`int`_ : The length of the received ADU. `-1` if the operation fails.

### `send()`

Sends a request to the server. The request is assembled into the `request` ADU. This function uses the `send()` function of the parent `CSE_ModbusRTU` object.

#### Syntax

```cpp
client.send();
```

##### Parameters

None

##### Returns

The return value comes from the `send()` function of the parent `CSE_ModbusRTU` object.

* _`int`_ : The length of the ADU sent. `-1` if the operation fails.

### `readCoil()`

Read one or more coils from the remote server. This function form the `request` message, sends it to the server and wait for a response. The response from the server is saved to the `response` ADU. The `response` ADU is checked for its type and the original function code is returned if the operation is successful. If the response ADU is an exception, the exception code is returned. If the operation fails for other reasons, `-1` returned.

#### Syntax

```cpp
client.readCoil (uint16_t address,  uint8_t count, uint8_t* coilValues);
```

##### Parameters

* `address` : The address of the coil to be read.
* `count` : The number of coils to be read.
* `coilValues` : A pointer to an array of `uint8_t` to store the coil values.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

### `writeCoil()`

Write one or more coils to the remote server. This function form the `request` message, sends it to the server and wait for a response. The response from the server is saved to the `response` ADU. The `response` ADU is checked for its type and the original function code is returned if the operation is successful. If the response ADU is an exception, the exception code is returned. If the operation fails for other reasons, `-1` returned.

#### Syntax 1

Writes a single coil.

```cpp
client.writeCoil (uint16_t address, uint8_t value);
```

##### Parameters

* `address` : The address of the coil to be written.
* `value` : The value of the coil to be written.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

#### Syntax 2

Writes multiple coils. The `coilValues` array must be of the same size as the `count` parameter. You must first create an `uint8_t` array of coil values. Each value in the array must be `0x00` or `0x01`.

```cpp
client.writeCoil (uint16_t address, uint8_t count,  uint8_t* coilValues);
```

##### Parameters

* `address` : The starting address of the coil to be written.
* `count` : The number of coils to be written.
* `coilValues` : A pointer to an array of `uint8_t` coil values to write. Each value must be `0x00` or `0x01`.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

### `readDiscreteInput()`

Read one or more discrete inputs from the remote server. This function form the `request` message, sends it to the server and wait for a response. The response from the server is saved to the `response` ADU. The `response` ADU is checked for its type and the original function code is returned if the operation is successful. If the response ADU is an exception, the exception code is returned. If the operation fails for other reasons, `-1` returned.

#### Syntax

```cpp
client.readDiscreteInput (uint16_t address,  uint8_t count, uint8_t* inputValues);
```

##### Parameters

* `address` : The starting address of the discrete input to be read.
* `count` : The number of discrete inputs to be read.
* `inputValues` : A pointer to an array of `uint8_t` to store the discrete input values.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

### `readInputRegister()`

Read one or more input registers from the remote server. This function form the `request` message, sends it to the server and wait for a response. The response from the server is saved to the `response` ADU. The `response` ADU is checked for its type and the original function code is returned if the operation is successful. If the response ADU is an exception, the exception code is returned. If the operation fails for other reasons, `-1` returned.

#### Syntax

```cpp
client.readInputRegister (uint16_t address,  uint8_t count, uint16_t* inputRegisters);
```

##### Parameters

* `address` : The starting address of the input register to be read.
* `count` : The number of input registers to be read.
* `inputRegisters` : A pointer to an array of `uint16_t` to store the input register values. Size of the array must be greater than or equal to `count`.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

### `readHoldingRegister()`

Read one or more holding registers from the remote server. This function form the `request` message, sends it to the server and wait for a response. The response from the server is saved to the `response` ADU. The `response` ADU is checked for its type and the original function code is returned if the operation is successful. If the response ADU is an exception, the exception code is returned. If the operation fails for other reasons, `-1` returned.

#### Syntax

```cpp
client.readHoldingRegister (uint16_t address,  uint8_t count, uint16_t* holdingRegisters);
```

##### Parameters

* `address` : The starting address of the holding register to be read.
* `count` : The number of holding registers to be read.
* `holdingRegisters` : A pointer to an array of `uint16_t` to store the holding register values. Size of the array must be greater than or equal to `count`.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

### `writeHoldingRegister()`

Write one or more holding registers to the remote server. This function form the `request` message, sends it to the server and wait for a response. The response from the server is saved to the `response` ADU. The `response` ADU is checked for its type and the original function code is returned if the operation is successful. If the response ADU is an exception, the exception code is returned. If the operation fails for other reasons, `-1` returned.

#### Syntax 1

Writes a single holding register.

```cpp
client.writeHoldingRegister (uint16_t address, uint16_t value);
```

##### Parameters

* `address` : The address of the holding register to be written.
* `value` : The value of the holding register to be written.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.

#### Syntax 2

Writes multiple holding registers. The `holdingRegisters` array must be of the same size as the `count` parameter. You must first create an `uint16_t` array of holding register values.

```cpp
client.writeHoldingRegister (uint16_t address, uint8_t count,  uint16_t* registerValues);
```

##### Parameters

* `address` : The starting address of the holding register to be written.
* `count` : The number of holding registers to be written.
* `registerValues` : A pointer to an array of `uint16_t` holding register values to write. The size of the array must be greater than or equal to `count`.

##### Returns

* _`int`_ :
  * The function code received from the server if the operation was successful.
  * The exception code received from the server if the operation was unsuccessful.
  * `-1` if the operation fails.


