
//======================================================================================//
/**
 * @file CSE_ModbusRTU.h
 * @brief Main header file for the CSE_ModbusRTU library.
 * @date +05:30 10:15:02 AM 26-02-2024, Monday
 * @version 0.0.8
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_ModbusRTU
 * @par MIT License
 * 
 */
//======================================================================================//

#ifndef CSE_MODBUSRTU_H
#define CSE_MODBUSRTU_H

#if defined(ARDUINO_ARCH_AVR)
  #include <ArduinoSTL.h>
#else
  #include <vector>
#endif

// You can define the type of serial port to use for the Modbus RTU node here.
#define   MODBUS_RTU_SERIAL_PORT_OBJECT   RS485Class

// Make a library selection based on the type of serial port selected.
#if (MODBUS_RTU_SERIAL_PORT_OBJECT == RS485Class)
  #include <CSE_ArduinoRS485.h>
#endif

//======================================================================================//

// Modbus constants and limits
#define   MODBUS_RTU_ADU_LENGTH_MAX                     256
#define   MODBUS_RTU_PDU_LENGTH_MAX                     253
#define   MODBUS_RTU_ADDR_LENGTH_MAX                    2
#define   MODBUS_RTU_CRC_LENGTH                         2
#define   MODBUS_RTU_ADU_ADDRESS_INDEX                  0
#define   MODBUS_RTU_ADU_FUNCTION_CODE_INDEX            1
#define   MODBUS_RTU_ADU_EXCEPTION_CODE_INDEX           2
#define   MODBUS_RTU_ADU_DATA_LENGTH_MAX                252  // Doesn't include the function code
#define   MODBUS_RTU_ADU_DATA_INDEX                     2
#define   MODBUS_RTU_COIL_COUNT_MAX                     100
#define   MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX           100
#define   MODBUS_RTU_INPUT_REGISTER_COUNT_MAX           100
#define   MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX         100

// Modbus function codes
#define   MODBUS_FC_READ_COILS                          0x01
#define   MODBUS_FC_READ_DISCRETE_INPUTS                0x02
#define   MODBUS_FC_READ_HOLDING_REGISTERS              0x03
#define   MODBUS_FC_READ_INPUT_REGISTERS                0x04
#define   MODBUS_FC_WRITE_SINGLE_COIL                   0x05
#define   MODBUS_FC_WRITE_SINGLE_REGISTER               0x06
#define   MODBUS_FC_READ_EXCEPTION_STATUS               0x07
#define   MODBUS_FC_WRITE_MULTIPLE_COILS                0x0F
#define   MODBUS_FC_WRITE_MULTIPLE_REGISTERS            0x10
#define   MODBUS_FC_REPORT_SERVER_ID                    0x11
#define   MODBUS_FC_MASK_WRITE_REGISTER                 0x16
#define   MODBUS_FC_WRITE_AND_READ_REGISTERS            0x17

// Modbus exception codes
#define   MODBUS_EX_ILLEGAL_FUNCTION                    0x01
#define   MODBUS_EX_ILLEGAL_DATA_ADDRESS                0x02
#define   MODBUS_EX_ILLEGAL_DATA_VALUE                  0x03
#define   MODBUS_EX_SERVER_DEVICE_FAILURE               0x04
#define   MODBUS_EX_ACKNOWLEDGE                         0x05
#define   MODBUS_EX_SERVER_DEVICE_BUSY                  0x06
#define   MODBUS_EX_NEGATIVE_ACKNOWLEDGE                0x07
#define   MODBUS_EX_MEMORY_PARITY_ERROR                 0x08
#define   MODBUS_EX_GATEWAY_PATH_UNAVAILABLE            0x0A
#define   MODBUS_EX_GATEWAY_TARGET_NO_RESPONSE          0x0B

//======================================================================================//

// You can change the serial port for debug messages here
#define   MODBUS_DEBUG_SERIAL      Serial

// Enable or disable the debug functionality here.
#define   ENABLE_DEBUG             1

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT_HELPER(condition, ...) \
    do { \
      if (condition) { \
          MODBUS_DEBUG_SERIAL.print(__VA_ARGS__); \
      } \
    } while (0)
#else
  #define DEBUG_PRINT_HELPER(condition, ...) ((void)0)
#endif

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINTLN_HELPER(condition, ...) \
    do { \
      if (condition) { \
          MODBUS_DEBUG_SERIAL.println(__VA_ARGS__); \
      } \
    } while (0)
#else
  #define DEBUG_PRINTLN_HELPER(condition, ...) ((void)0)
#endif

#define DEBUG_PRINT(...) DEBUG_PRINT_HELPER(CSE_ModbusRTU_Debug::debugEnabled, __VA_ARGS__)
#define DEBUG_PRINTLN(...) DEBUG_PRINTLN_HELPER(CSE_ModbusRTU_Debug::debugEnabled, __VA_ARGS__)

//======================================================================================//
// Forward declarations

class CSE_ModbusRTU_ADU;
class CSE_ModbusRTU;
class CSE_ModbusRTU_Server;
class CSE_ModbusRTU_Client;
class CSE_ModbusRTU_Debug;

//======================================================================================//

// Define the DebugUtils class
class CSE_ModbusRTU_Debug {
public:
  // Function to enable debug messages
  static void enableDebugMessages() {
    debugEnabled = true;
  }

  // Function to disable debug messages
  static void disableDebugMessages() {
    debugEnabled = false;
  }

  // Friend class declaration
  friend class CSE_ModbusRTU_ADU; // Add more friend classes as needed
  friend class CSE_ModbusRTU;
  friend class CSE_ModbusRTU_Server;
  friend class CSE_ModbusRTU_Client;

private:
  // Variable to track debug state
  static bool debugEnabled;
};

//======================================================================================//
/**
 * @brief A generic class to store a Modbus RTU ADU (Application Data Unit).
 * 
 */
class CSE_ModbusRTU_ADU {
  private:
    uint8_t aduBuffer [MODBUS_RTU_ADU_LENGTH_MAX];  // The ADU buffer for transmitting
    uint8_t aduLength;  // The number of valid bytes in the receive ADU buffer

  public:
    enum aduType_t {
      NONE,
      REQUEST,
      RESPONSE,
      EXCEPTION
    } aduType;  // The type of ADU
    
    CSE_ModbusRTU_ADU();

    bool resetLength(); // Reset the ADU length to 0
    uint8_t getLength(); // Get the ADU length

    bool clear(); // Clear the ADU buffer by setting all bytes to 0x00
    bool clear (uint8_t index); // Clear a byte in the ADU buffer by setting it to 0x00
    bool clear (uint8_t index, uint8_t length); // Clear a range of bytes in the ADU buffer by setting them to 0x00

    bool add (uint8_t byte); // Add a byte to the ADU buffer
    bool add (uint8_t* buffer, uint8_t length); // Add a buffer of bytes to the ADU buffer
    bool add (uint16_t word); // Add a word to the ADU buffer
    bool add (uint16_t* buffer, uint8_t length); // Add a buffer of words to the ADU buffer

    bool checkCRC(); // Check the CRC of the ADU buffer
    uint16_t calculateCRC (bool isCRCSet = false); // Calculate the CRC of the ADU

    bool setType (int type); // Set the type of the ADU
    bool setDeviceAddress (uint8_t address); // Set the device address of the ADU
    bool setFunctionCode (uint8_t functionCode); // Set the function code of the ADU
    bool setException(); // Convert the function code to an exception
    bool setExceptionCode (uint8_t exceptionCode); // Set the exception code of the ADU
    bool setData (uint8_t* buffer, uint8_t length); // Set the data of the ADU
    uint16_t setCRC(); // Calculate the CRC of the ADU and add it to the buffer

    uint8_t getDeviceAddress(); // Get the device address of the ADU
    uint8_t getFunctionCode(); // Get the function code of the ADU
    uint8_t getExceptionCode(); // Get the exception code of the ADU
    uint16_t getStartingAddress(); // Get the starting address of the ADU
    uint16_t getQuantity(); // Get the quantity (register count) of the ADU
    uint16_t getCRC(); // Get the CRC of the ADU
    uint8_t getDataLength(); // Get the data length of the ADU
    int getType(); // Get the type of the ADU

    uint8_t getByte (uint8_t index); // Get a byte from the ADU buffer
    uint16_t getWord (uint8_t index); // Get a word from the ADU buffer

    void print(); // Print the ADU buffer to the serial port
};

//======================================================================================//
/**
 * @brief Generic Modbus RTU class. Implements common functions and data structures
 * needed for both Modbus RTU server and client.
 * 
 */
class CSE_ModbusRTU {
  private:
    // friend class CSE_ModbusRTU_Server;
    // friend class CSE_ModbusRTU_Client;

    bool setServer (CSE_ModbusRTU_Server& server);
    bool setClient (CSE_ModbusRTU_Client& client);

    String name; // The name of the Modbus RTU object

  public:
    int enableReceive (bool deassertDE = false); // Enable receiving Modbus RTU packets. Asserts RE. DE is optional.
    int disableReceive(); // Disable receiving Modbus RTU packets. De-asserts RE. DE is not affected.
    int receive (CSE_ModbusRTU_ADU& adu, uint32_t timeout = 100);  // Receive a custom Modbus RTU packet
    int send (CSE_ModbusRTU_ADU& adu); // Send a custom Modbus RTU packet

    /**
     * @brief This typedef defines the serial port object used for Modbus RTU communication.
     * It can be any object that implements the following methods:
     * - begin (baudrate)
     * - available ()
     * - read ()
     * - write (byte)
     * - flush ()
     * - peek ()
     * - end ()
     */
    typedef MODBUS_RTU_SERIAL_PORT_OBJECT* serialPort_t;

    serialPort_t serialPort; // The serial port used for Modbus RTU communication

    // It is important to not get confused by the device address variables.
    // A deviceAddress is the address of the host device itself. It can be a client or a server.
    // A remoteDeviceAddress is the address of the remote device that the host device is communicating with.
    // This can also be a client or a server.

    uint8_t deviceAddress; // The Modbus RTU device id (1-247). Can be client or server.
    uint8_t remoteDeviceAddress; // The remote Modbus RTU device id (1-247). Can be client or server.

    // You can have only one server and/or client per RTU.
    // If you want to have multiple servers or clients, you need to create multiple RTUs.
    // It is possible to have client and server at the same time.
    // But it's up to you to manage their roles and communication.
    CSE_ModbusRTU_Server* server; // Pointer to the server object connected to this RTU
    CSE_ModbusRTU_Client* client; // Pointer to the client object connected to this RTU

    CSE_ModbusRTU (serialPort_t serialPort, uint8_t deviceAddress, String name);
    String getName();
};

//======================================================================================//
/**
 * @brief A custom type for storing Modbus RTU bits. It can be used for coils and
 * discrete inputs.
 * 
 */
class modbus_bit_t {
  public:
    uint16_t address;
    uint8_t value;

    modbus_bit_t (uint16_t address, uint8_t value) {
      this->address = address;
      this->value = value;
    }
};

//======================================================================================//
/**
 * @brief A custom type for storing Modbus RTU registers. It can be used for holding
 * registers and input registers.
 * 
 */
class modbus_register_t {
  public:
    uint16_t address;
    uint16_t value;

    modbus_register_t (uint16_t address, uint16_t value) {
      this->address = address;
      this->value = value;
    }
};

//======================================================================================//
/**
 * @brief Implements the Modbus RTU server node. You first need to create an instance of
 * CSE_ModbusRTU class and then pass it to the constructor of this class. Since the server
 * is what maintains the data, you can access and modify Modbus data with this class.
 * 
 */
class CSE_ModbusRTU_Server {
  private:
    String name;  // The name of the server
    CSE_ModbusRTU* rtu; // The parent RTU object

  public:
    // The following vectors store the Modbus data.
    std::vector <modbus_bit_t> coils;
    std::vector <modbus_bit_t> discreteInputs;
    std::vector <modbus_register_t> holdingRegisters;
    std::vector <modbus_register_t> inputRegisters;

    // There are two ADUs, one for request and one for response.
    // request ADUs are sent by the client.
    // and response is used to send data to the client.
    CSE_ModbusRTU_ADU request; // The request ADU
    CSE_ModbusRTU_ADU response; // The response ADU

    CSE_ModbusRTU_Server (CSE_ModbusRTU& rtu, String name);

    String getName(); // Returns the name of the server

    bool begin(); // Does nothing for now.
    int poll(); // Listen for incoming requests from the client and process them
    int receive(); // Receive a request from the client
    int send(); // Send a response to the client

    // The following functions are used to configure and read Modbus data.
    bool configureCoils (uint16_t startAddress, uint16_t count); // Create and add new coils to the server
    bool configureDiscreteInputs (uint16_t address, uint16_t count); // Create and add new discrete inputs to the server
    bool configureInputRegisters (uint16_t address, uint16_t count); // Create and add new input registers to the server
    bool configureHoldingRegisters (uint16_t address, uint16_t count); // Create and add new holding registers to the server

    int readCoil (uint16_t address); // Read a single coil from the server itself
    int writeCoil (uint16_t address, uint8_t value); // Write a single coil to the server itself
    int writeCoil (uint16_t address, uint8_t value, uint16_t count); // Write multiple coils to the server itself
    bool isCoilPresent (uint16_t address); // Check if a coil is present in the server
    bool isCoilPresent (uint16_t address, uint16_t count); // Check if multiple coils are present in the server

    int readDiscreteInput (uint16_t address); // Read a single discrete input from the server itself
    int writeDiscreteInput (uint16_t address, uint8_t value); // Write a single discrete input to the server itself
    int writeDiscreteInput (uint16_t address, uint8_t value, uint16_t count); // Write multiple discrete inputs to the server itself
    bool isDiscreteInputPresent (uint16_t address); // Check if a discrete input is present in the server
    bool isDiscreteInputPresent (uint16_t address, uint16_t count); // Check if multiple discrete inputs are present in the server

    int readInputRegister (uint16_t address); // Read a single input register from the server itself
    int writeInputRegister (uint16_t address, uint16_t value); // Write a single input register to the server itself
    int writeInputRegister (uint16_t address, uint16_t value, uint16_t count); // Write multiple input registers to the server itself
    bool isInputRegisterPresent (uint16_t address); // Check if an input register is present in the server
    bool isInputRegisterPresent (uint16_t address, uint16_t count); // Check if multiple input registers are present in the server

    int readHoldingRegister (uint16_t address); // Read a single holding register from the server itself
    int writeHoldingRegister (uint16_t address, uint16_t value); // Write a single holding register to the server itself
    int writeHoldingRegister (uint16_t address, uint16_t value, uint16_t count); // Write multiple holding registers to the server itself
    bool isHoldingRegisterPresent (uint16_t address); // Check if a holding register is present in the server
    bool isHoldingRegisterPresent (uint16_t address, uint16_t count); // Check if multiple holding registers are present in the server
};

//======================================================================================//

class CSE_ModbusRTU_Client {
  private:
    String name; // The name of the client
    CSE_ModbusRTU* rtu; // The parent RTU object
  
  public:

    // There are two ADUs, one for request and one for response.
    // request is used to send a request to the the server.
    // and response is the response ADU from the server.
    CSE_ModbusRTU_ADU request; // The request sent to the server
    CSE_ModbusRTU_ADU response; // The response from the server

    uint32_t receiveTimeout = 1000; // The timeout for receiving a response from the server

    CSE_ModbusRTU_Client (CSE_ModbusRTU& rtu, String name);

    bool begin();
    int receive(); // Receive a response from the server
    int send(); // Send a request to the server

    bool setServerAddress (uint8_t remoteAddress); // Set the address of the server (0x00 to 0xFF)
    String getName(); // Returns the name of the server

    int readCoil (uint16_t address,  uint8_t count, uint8_t* coilValues); // Read a single coil from the server
    int writeCoil (uint16_t address, uint16_t value); // Write a single coil to the server
    int writeCoil (uint16_t address, uint16_t count, uint8_t* coilValues); // Write multiple coils to the server

    int readDiscreteInput (uint16_t address, uint8_t count, uint8_t* inputValues); // Read a single discrete input from the server

    int readInputRegister (uint16_t address, uint8_t count, uint16_t* inputRegisters); // Read a single input register from the server

    int readHoldingRegister (uint16_t address, uint8_t count, uint16_t* holdingRegisters); // Read a single holding register from the server
    int writeHoldingRegister (uint16_t address, uint16_t value); // Write a single holding register to the server
    int writeHoldingRegister (uint16_t address, uint16_t count, uint16_t* registerValues); // Write multiple holding registers to the server
};

#endif

//======================================================================================//

