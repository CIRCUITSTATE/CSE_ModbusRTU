
![CSE_ModbusRTU](https://socialify.git.ci/CIRCUITSTATE/CSE_ModbusRTU/image?description=1&font=KoHo&forks=1&issues=1&logo=https%3A%2F%2Fwww.circuitstate.com%2Fwp-content%2Fuploads%2F2024%2F05%2FCIRCUITSTATE-R-Emblem-20052024-2.svg&name=1&pattern=Circuit%20Board&pulls=1&stargazers=1&theme=Auto)

# CSE_ModbusRTU

**CSE_ModbusRTU** is an Arduino library from [*CIRCUITSTATE Electronics*](https://www.circuitstate.com/). This library allows you to implement the **Modbus RTU** protocol on any Arduino-supported microcontroller board. It supports both client and server modes. The library depends on the [**CSE_ArduinoRS485**](https://github.com/CIRCUITSTATE/CSE_ArduinoRS485) library from CIRCUITSTATE Electronics. You can use both hardware and software serial ports for the communication interface.

This library is in pre-release stage and any feedback is welcome.

## Installation

This library is available from the official **Arduino Library Manager**. Open the Arduino IDE, search for "CSE_ModbusRTU" and install the latest version of the library.

Additionally, you can download the latest release package from the GitHub repository and install it manually. To do so, open the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library…` and select the downloaded file.

Another method is to clone the GitHub repository directly into your `libraries` folder. The development branch will have the latest features, bug fixes and other changes. To do so, navigate to your `libraries` folder (usually located at `Documents/Arduino/libraries` on Windows and `~/Documents/Arduino/libraries` on macOS) and execute the following command:

```
git clone https://github.com/CIRCUITSTATE/CSE_ModbusRTU.git
```

[**Git**](https://git-scm.com) should be installed on your computer.

The library can also be installed via [**PlatformIO**](https://platformio.org). All officially listed Arduino listed libraries are automatically fetched by PlatformIO. Use the [`lib_deps`](https://docs.platformio.org/en/latest/projectconf/sections/env/options/library/lib_deps.html) search option to install the library.

## Dependencies

This library depends on the following libraries:

  - [**CSE_ArduinoRS485**](https://github.com/CIRCUITSTATE/CSE_ArduinoRS485)

## Examples

Four examples are included with this library:

  - **ModbusRTU_Client_LED** - Acts as a Modbus RTU Client and sends periodic requests to a Modbus RTU Server to control an LED via Coil data.
  - **ModbusRTU_Server_LED** - Acts as a Modbus RTU Server and responds to requests from a Modbus RTU Client to control an LED using Coil data.
  - **Holding_Register_Client** - Acts as a Modbus RTU Server and responds to requests from a Modbus RTU Client to control an LED using Coil data.
  - **Holding_Register_Server** - Acts as a Modbus RTU Server and responds to requests from a Modbus RTU Client to control an LED using Coil data.

The examples are categorized for three different target boards.

  - [**ESP32**](/examples/ESP32/) (Uses hardware serial port)
  - [**RP2040**](/examples/RP2040/) (Uses hardware serial port)
  - [**ESP8266**](/examples/ESP8266/) (Uses software serial port)

## API Reference

Please see the [API.md](/docs/API.md) file for the API reference.

## References

- [Modbus RTU tutorial by CIRCUITSTATE](https://www.circuitstate.com/tutorials/what-is-modbus-communication-protocol-and-how-to-implement-modbus-rtu-with-arduino/)
- [RS-485 tutorial by CIRCUITSTATE](https://www.circuitstate.com/tutorials/what-is-rs-485-how-to-use-max485-with-arduino-for-reliable-long-distance-serial-communication/)
- [MAX485+CD4069 Module Pinout by CIRCUITSTATE](https://www.circuitstate.com/pinouts/max485-cd4069-rs-485-module-with-auto-data-direction-control-pinout-diagram-and-pin-reference/)