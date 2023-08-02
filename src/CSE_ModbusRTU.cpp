
//======================================================================================//
/**
 * @file CSE_ModbusRTU.cpp
 * @brief Main source file for the CSE_ModbusRTU library.
 * @date +05:30 04:45:28 PM 02-08-2023, Wednesday
 * @version 0.0.1
 * @author Vishnu Mohanan (@vishnumaiea)
 * @par GitHub Repository: https://github.com/CIRCUITSTATE/CSE_ModbusRTU
 * @par MIT License
 * 
 */
//======================================================================================//

#include "CSE_ModbusRTU.h"

//======================================================================================//
/**
 * @brief Create a new CSE_ModbusRTU_ADU object. The ADU buffer is cleated during
 * instantiation.
 * 
 */
CSE_ModbusRTU_ADU::CSE_ModbusRTU_ADU() {
  clear();
}

//======================================================================================//
/**
 * @brief Resets the aduLength to 0 without clearing any data on the buffer. This is
 * useful for a faster write to the buffer without waiting for the buffer to be cleared.
 * Resetting the aduLength will prvent you from doing any operation on the buffer even
 * if it has valid data. So use this function with caution, and only before you start
 * writing to the buffer.
 * 
 * @return true 
 * @return false 
 */
bool CSE_ModbusRTU_ADU:: resetLength() {
  aduLength = 0;
  return true;
}

//======================================================================================//
/**
 * @brief Returns the number of valid bytes in the ADU buffer. A value 0 indicates that
 * the buffer has no valid data even though its contents may not be all 0x00.
 * 
 * @return uint8_t 
 */
uint8_t CSE_ModbusRTU_ADU:: getLength() {
  return aduLength;
}

//======================================================================================//
/**
 * @brief Clear the ADU buffer by setting all bytes to 0x00.
 * 
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: clear() {
  return clear (0, MODBUS_RTU_ADU_LENGTH_MAX);
}

//======================================================================================//
/**
 * @brief Clear a single byte in the ADU buffer by setting it to 0x00.
 * 
 * @param index The index of the byte to clear.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: clear (uint8_t index) {
  return clear (index, 1);
}

//======================================================================================//
/**
 * @brief Clear a range of bytes in the ADU buffer by setting them to 0x00. So what
 * happens to aduLength when you clear random bytes? The aduLength is not changed.
 * This function allows you to shoot yourself in the foot. So unless you know what
 * you are doing you should not use this function.
 * 
 * @param index The starting index of the bytes to clear.
 * @param length The number of bytes to clear.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: clear (uint8_t index, uint8_t length) {
  // Check if the length is valid
  if ((index + length) > MODBUS_RTU_ADU_LENGTH_MAX) {
    return false;
  }

  for (uint8_t i = index; i < (index + length); i++) {
    aduBuffer [i] = 0x00;
  }

  return true;
}

//======================================================================================//
/**
 * @brief Add a byte to the ADU buffer. The new byte is written to the end of the
 * buffer indicated by aduLength. The aduLength is incremented by 1.
 * 
 * @param byte The byte to add.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: add (uint8_t byte) {
  if (aduLength >= MODBUS_RTU_ADU_LENGTH_MAX) {
    return false;
  }

  aduBuffer [aduLength++] = byte;

  return true;
}

//======================================================================================//
/**
 * @brief Add a byte array to the ADU buffer. The new bytes are written to the end of
 * the buffer indicated by aduLength. The aduLength is incremented by the length of
 * the byte array.
 * 
 * @param buffer A byte buffer to add.
 * @param length The number of bytes to add.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: add (uint8_t* buffer, uint8_t length) {
  if ((aduLength + length) > MODBUS_RTU_ADU_LENGTH_MAX) {
    return false;
  }

  for (uint8_t i = 0; i < length; i++) {
    aduBuffer [aduLength++] = buffer [i];
  }

  return true;
}

//======================================================================================//
/**
 * @brief Add a word (16-bit) to the ADU buffer. The new word is written to the end of\
 * the buffer indicated by aduLength. The aduLength is incremented by 2.
 * 
 * @param word A 16-bit data word to add.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: add (uint16_t word) {
  if ((aduLength + 2) > MODBUS_RTU_ADU_LENGTH_MAX) {
    return false;
  }

  aduBuffer [aduLength++] = (uint8_t) (word >> 8);
  aduBuffer [aduLength++] = (uint8_t) (word & 0xFF);

  return true;
}

//======================================================================================//
/**
 * @brief Add a word array to the ADU buffer. The new words are written to the end of
 * the buffer indicated by aduLength. The aduLength is incremented by the length of
 * the word array times 2.
 * 
 * @param buffer A 16-bit data buffer to add.
 * @param length The number of 16-bit data words to add.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: add (uint16_t* buffer, uint8_t length) {
  if ((aduLength + (length * 2)) > MODBUS_RTU_ADU_LENGTH_MAX) {
    return false;
  }

  // Split the words and add them to the buffer
  for (uint8_t i = 0; i < length; i++) {
    aduBuffer [aduLength++] = (uint8_t) (buffer [i] >> 8);
    aduBuffer [aduLength++] = (uint8_t) (buffer [i] & 0xFF);
  }

  return true;
}

//======================================================================================//
/**
 * @brief Calculates the CRC of the ADU and compares it to the CRC in the ADU.
 * 
 * @return true - If the CRCs match.
 * @return false - If the CRCs do not match.
 */
bool CSE_ModbusRTU_ADU:: checkCRC() {
  // If the ADU length is less than 3, that means that the device address, function code,
  // and data are not set yet. In this case, we can't set the data.
  if (aduLength < 3) {
    return 0x0000;
  }

  uint16_t crc = calculateCRC();

  if ((aduBuffer [aduLength - 2] == (uint8_t) (crc & 0xFF)) && (aduBuffer [aduLength - 1] == (uint8_t) (crc >> 8))) {
    return true;
  }
  else {
    return false;
  }
}

//======================================================================================//
/**
 * @brief Calculates the CRC of the ADU contents and returns it. If the aduLength is
 * not sufficient to calculate the CRC then 0x00 is returned.
 * 
 * @return uint16_t The CRC of the ADU contents.
 */
uint16_t CSE_ModbusRTU_ADU:: calculateCRC() {
  // If the ADU length is less than 3, that means that the device address, function code,
  // and data are not set yet. In this case, we can't set the data.
  if (aduLength < 3) {
    return 0x0000;
  }

  uint16_t crc = 0xFFFF;

  for (uint8_t i = 0; i < (aduLength - MODBUS_RTU_CRC_LENGTH); i++) {
    crc ^= aduBuffer [i];

    for (uint8_t j = 0; j < 8; j++) {
      if ((crc & 0x0001) == 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }

  return crc;
}

//======================================================================================//
/**
 * @brief Set the type of the ADU. Setting the type does not check if the data in the
 * ADU is valid. For example, if you are setting the type to EXCEPTION, this function
 * doesn't check if the function code is valid for an exception. So you should only
 * use this function after you have set the ADU data correctly.
 * 
 * @param type A valid ADU type.
 * @return true - Operation successful.
 * @return false - Operation failed due to invalid type.
 */
bool CSE_ModbusRTU_ADU:: setType (int type) {
  switch (type) {
    case aduType_t:: NONE:
    case aduType_t:: REQUEST:
    case aduType_t:: RESPONSE:
    case aduType_t:: EXCEPTION:
      aduType = (aduType_t) type;
      return true;
    default:
      return false;
  }
}

//======================================================================================//
/**
 * @brief Set the device address field on the ADU. The device address can be client or
 * server address depending on where it is used. Ideally, you should set the device
 * address first before setting other fields. The ADU length will be incremented only
 * if the current length is less than 2. If you set the device address after setting
 * other fields, the ADU length will not be incremented.
 * 
 * @param address Client or server address.
 * @return true Operation successful.
 * @return false Operation failed.
 */
bool CSE_ModbusRTU_ADU:: setDeviceAddress (uint8_t address) {
  aduBuffer [MODBUS_RTU_ADU_ADDRESS_INDEX] = address;

  // If the ADU length is already greater than 2, we don't need to increment the length further.
  // For example when we have already set the function code, we don't need to increment the length
  // when setting the device address next. 
  if (aduLength < 2) {
    aduLength++;
  }

  return true;
}

//======================================================================================//
/**
 * @brief Set the function code field on the ADU. The value of the function code should
 * be from 0x01 to 0x7F. The ADU length is incremented only if the current length is less
 * than 2. Ideally, you should set the function code after setting the device address.
 * 
 * @param functionCode Valid function code from 0x01 to 0x7F (1 - 127)
 * @return true Operation successful.
 * @return false Operation failed.
 */
bool CSE_ModbusRTU_ADU:: setFunctionCode (uint8_t functionCode) {
  // Valid function code should be from 0x01 to 0x7F.
  if (functionCode >= 0x80) {
    return false;
  }

  aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX] = functionCode;

  // If the ADU length is already greater than 2, we don't need to increment the length further.
  if (aduLength < 2) {
    aduLength++;
  }
  return true;
}

//======================================================================================//
/**
 * @brief Set the exception code just after the function code. The aduLength should be
 * exactly 2 for this to work. So you must set the device address and function code
 * first before setting the exception code. The aduLength is incremented by 1.
 * 
 * @param exceptionCode Valid exception code from 0x01 to 0x7F.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: setExceptionCode (uint8_t exceptionCode) {
  // We will only set the exception code if the device address and function code are already set.
  // This is because the exception code is always set after the function code.
  if (aduLength != 2) {
    return false;
  }

  // Valid excpetion code should be from 0x01 to 0x7F.
  if (exceptionCode >= 0x80) {
    return false;
  }

  aduBuffer [MODBUS_RTU_ADU_EXCEPTION_CODE_INDEX] = exceptionCode;
  aduLength++;  // Length should now be 3
  return true;
}

//======================================================================================//
/**
 * @brief Convert the function code to an exception by setting the MSB to 1. Will not
 * work if the function code is already an exception (>= 0x80). aduLength is not changed.
 * 
 * @return true 
 * @return false 
 */
bool CSE_ModbusRTU_ADU:: setException() {
  // If the ADU length is less than 2, that means that the device address and function code
  // are not set yet. In this case, we can't set the exception.
  if (aduLength < 2) {
    return false;
  }

  // Converting a function code to an exception is done simply by setting he MSB to 1.
  if (aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX] < 0x80) { // Check if the function code is not already an exception
    aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX] += 0x80; // Set the function code as an exception
    return true;
  }

  return false;
}

//======================================================================================//
/**
 * @brief Set the data field of the ADU. The data will be set right after the end of
 * the function code field. You must set the device address and function code first.
 * Otherwise the operation fails. You are free to change the data later after setting
 * it successfully once. This will rewrite existing data. The data length should not
 * exceed the maximum limit. Otherwise the operation fails.
 * 
 * You should also calculate and set the CRC right after writing the data.
 * 
 * @param buffer A pointer to the data buffer.
 * @param length The length of data to write in bytes.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: setData (uint8_t* buffer, uint8_t length) {
  // If the ADU length is less than 2, that means that the device address and function code
  // are not set yet. In this case, we can't set the data.
  if (aduLength < 2) {
    return false;
  }

  // Data length should not exceed the limit.
  if (length > MODBUS_RTU_ADU_DATA_LENGTH_MAX) {
    return false;
  }

  aduLength = MODBUS_RTU_ADU_DATA_INDEX;

  for (uint8_t i = 0; i < length; i++) {
    aduBuffer [aduLength++] = buffer [i];
  }

  return true;
}

//======================================================================================//
/**
 * @brief Calculate the CRC based on the data in the ADU, and write it at the end of
 * the ADU. You must set valid device address, function code and data before setting
 * the CRC field. If the ADU length is less than 3, the operation fails and returns
 * 0x00, which is not a valid CRC. The calculated CRC is also returned. You must check
 * the return value if you want to be sure the CRC is set correctly.
 * 
 * @return uint16_t - The calculated CRC value.
 */
uint16_t CSE_ModbusRTU_ADU:: setCRC() {
  // If the ADU length is less than 3, that means that the device address, function code,
  // and data are not set yet. In this case, we can't set the data.
  if (aduLength < 3) {
    return 0x0000;
  }

  uint16_t crc = calculateCRC();

  aduBuffer [aduLength++] = (uint8_t) (crc & 0xFF); // Low byte
  aduBuffer [aduLength++] = (uint8_t) (crc >> 8); // High byte

  return crc;
}

//======================================================================================//
/**
 * @brief Returns the device address field data from the ADU. The address can be client
 * or server address depending on what you are using the ADU for.
 * 
 * @return uint8_t - The device address byte.
 */
uint8_t CSE_ModbusRTU_ADU:: getDeviceAddress() {
  return aduBuffer [MODBUS_RTU_ADU_ADDRESS_INDEX];
}

//======================================================================================//
/**
 * @brief Returns the function code from the ADU.
 * 
 * @return uint8_t - The function code byte.
 */
uint8_t CSE_ModbusRTU_ADU:: getFunctionCode() {
  return aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX];
}

//======================================================================================//
/**
 * @brief Returns the exception code from the ADU. The exception code is found by
 * subtracting 0x80 from the function code. If the ADU type is not an exception,
 * or if the function code is not valid, the function returns 0x00 which is not a
 * valid exception code. The function code has to be greater than 0x80 to be a valid
 * exception code. 
 * 
 * @return uint8_t - The exception code. 0x00 if no valid exception code.
 */
uint8_t CSE_ModbusRTU_ADU:: getExceptionCode() {
  if (aduType == aduType_t::EXCEPTION) { // Check if the ADU is an exception
    if (aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX] > 0x80) { // Check if the exception code is valid
      return aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX] - 0x80;
    }
  }

  return 0x00; // No exception code
}

//======================================================================================//
/**
 * @brief Returns the starting address field in the ADU. The function only reads the
 * value at the position, and does not guarantee that the value is valid.
 * 
 * @return uint16_t - The starting address in the ADU.
 */
uint16_t CSE_ModbusRTU_ADU:: getStartingAddress() {
  // Starting address always comes first in the data.
  return getWord (MODBUS_RTU_ADU_DATA_INDEX);
}

//======================================================================================//
/**
 * @brief Returns the quantity field in the ADU. The function only reads the value at
 * the position, and does not guarantee that the value is valid.
 * 
 * @return uint16_t - The quantity in the ADU.
 */
uint16_t CSE_ModbusRTU_ADU:: getQuantity() {
  // Quantity always comes after the starting address in the data.
  return getWord (MODBUS_RTU_ADU_DATA_INDEX + 2);
}

//======================================================================================//
/**
 * @brief Returns the CRC field from the ADU.
 * 
 * @return uint16_t - CRC value.
 */
uint16_t CSE_ModbusRTU_ADU:: getCRC() {
  return (uint16_t) (aduBuffer [aduLength - 2] << 8) + aduBuffer [aduLength - 1];
}

//======================================================================================//
/**
 * @brief Returns the length of data field in the ADU. This is the number of bytes
 * after the function code and before the CRC. The length is calculated purely from
 * the aduLength. So if the aduLength is not correct for some reason, the function
 * will return an incorrect value.
 * 
 * @return uint8_t - The length of data field in the ADU.
 */
uint8_t CSE_ModbusRTU_ADU:: getDataLength() {
  // Return the number of bytes in the ADU after device address, function code and CRC
  return (aduLength - MODBUS_RTU_ADU_DATA_INDEX - MODBUS_RTU_CRC_LENGTH); // aduLength - 4
}

//======================================================================================//
/**
 * @brief Returns a single byte from the ADU. The index should be within the aduLength.
 * If the index is not valid (greater than aduLength), the function returns 0x00. So
 * this guarantees that the function will always return a valid byte.
 * 
 * @param index The index of the byte to return.
 * @return uint8_t - The byte at the specified index.
 */
uint8_t CSE_ModbusRTU_ADU:: getByte (uint8_t index) {
  if (index < aduLength) {
    return aduBuffer [index];
  }
  else {
    return 0x00;
  }
}

//======================================================================================//
/**
 * @brief Returns the 16-bit word at the specified index. The index should be within
 * the aduLength. If the index is not valid (greater than aduLength), the function
 * returns 0x0000. So this guarantees that the function will always return a valid
 * word. Two consecutive bytes are combined to form the word. The index indicates the
 * position of the Hi byte.
 * 
 * @param index The starting index of the word to return.
 * @return uint16_t - The 16-bit word at the specified index.
 */
uint16_t CSE_ModbusRTU_ADU:: getWord (uint8_t index) {
  if ((index + 1) < aduLength) {
    return (uint16_t) (aduBuffer [index] << 8) + aduBuffer [index + 1];
  }
  else {
    return 0x0000;
  }
}

//======================================================================================//
/**
 * @brief Returns the current type of the ADU.
 * 
 * @return int The ADU type.
 */
int CSE_ModbusRTU_ADU:: getType() {
  return aduType;
}

//======================================================================================//
/**
 * @brief Instantiate a new CSE_ModbusRTU object. The device address is the host
 * address. The default request and response ADU types are set to NONE.
 * 
 * @param serialPort The serial port to read/write data.
 * @param deviceAddress The 8-bit device address.
 * @return CSE_ModbusRTU:: 
 */
CSE_ModbusRTU:: CSE_ModbusRTU (serialPort_t serialPort, uint8_t deviceAddress, String name) {
  this->serialPort = serialPort;
  this->deviceAddress = deviceAddress;
  this->name = name;
}

//======================================================================================//
/**
 * @brief Returns the name of the Modbus RTU object.
 * 
 * @return String 
 */
String CSE_ModbusRTU:: getName() {
  return name;
}

//======================================================================================//
/**
 * @brief This allows you to add a new Modbus RTU server to the Modbus RTU object.
 * A server is able to respond to Modbus RTU requests. You can have only one server
 * and client per Modbus RTU object. 
 * 
 * @param server A valid CSE_ModbusRTU_Server object.
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU:: setServer (CSE_ModbusRTU_Server& server) {
  this->server = &server;
  return true;
}

//======================================================================================//
/**
 * @brief This allows you to add a new Modbus RTU client to the Modbus RTU object.
 * A client is able to send Modbus RTU requests. You can have only one server and
 * client per Modbus RTU object.
 * 
 * @param client 
 * @return true 
 * @return false 
 */
bool CSE_ModbusRTU:: setClient (CSE_ModbusRTU_Client& client) {
  this->client = &client;
  return true;
}

//======================================================================================//
/**
 * @brief Reads the serial port and save an incoming ADU to the specified ADU object.
 * The aduLength is reset to 0 before reading the serial port. The function will check
 * the CRC of the received ADU and return the length of the ADU if the CRC is valid.
 * The address of the ADU is not checked. It has to be checked by the server or client.
 * 
 * @param adu 
 * @return int 
 */
int CSE_ModbusRTU:: receive (CSE_ModbusRTU_ADU& adu) {
  adu.resetLength(); // Reset the ADU length
  // DEBUG_SERIAL.print (F("receive(): Checking Modbus port.."));
  
  uint32_t startTime = millis();

  while ((millis() - startTime) < 100) {
    // Read all the bytes from the serial port
    while ((serialPort->available() > 0) && (adu.getLength() < MODBUS_RTU_ADU_LENGTH_MAX)) {
      adu.add ((uint8_t) serialPort->read()); // Read the first byte
    }
  }

  if (adu.getLength() > 0) {
    DEBUG_SERIAL.print (F("receive(): Received ADU:"));
    for (int i = 0; i < adu.getLength(); i++) {
      DEBUG_SERIAL.print (" ");
      if (adu.getByte (i) < 0x10) {
        DEBUG_SERIAL.print (F("0x0"));
      }
      else {
        DEBUG_SERIAL.print (F("0x"));
      }
      DEBUG_SERIAL.print (adu.getByte (i), HEX);
    }

    DEBUG_SERIAL.println();
  }

  // Now check if the ADU is valid. We can do this by simply checking the CRC of the ADU.
  if (adu.getLength() > 0) {
    if (adu.checkCRC()) { // Check the CRC of the ADU
      DEBUG_SERIAL.println (F("receive(): ADU CRC passed"));
      return (int) adu.getLength(); // Return the length of the ADU
    }
    else {
      DEBUG_SERIAL.println (F("receive(): ADU CRC failed"));
    }
  }
  
  return -1;
}

//======================================================================================//

int CSE_ModbusRTU:: send (CSE_ModbusRTU_ADU& adu) {
  // Check if the ADU is valid
  if (adu.checkCRC()) {
    DEBUG_SERIAL.print (F("send(): Sending ADU: 0x"));

    for (int i = 0; i < adu.getLength(); i++) {
      DEBUG_SERIAL.print (" ");
      if (adu.getByte (i) < 0x10) {
        DEBUG_SERIAL.print (F("0x0"));
      }
      else {
        DEBUG_SERIAL.print (F("0x"));
      }
      DEBUG_SERIAL.print (adu.getByte (i), HEX);
    }

    DEBUG_SERIAL.println();

    // Send the ADU
    for (uint8_t i = 0; i < adu.getLength(); i++) {
      serialPort->write (adu.getByte (i));
    }

    return adu.getLength(); // Return the length of the ADU
  }

  return -1;
}

//======================================================================================//

CSE_ModbusRTU_Server:: CSE_ModbusRTU_Server (CSE_ModbusRTU& rtu, String name) {
  this->rtu = &rtu;
  this->name = name;

  // Set the default request and response ADU types
  request.setType (CSE_ModbusRTU_ADU::aduType_t:: REQUEST);
  response.setType (CSE_ModbusRTU_ADU::aduType_t:: RESPONSE);

  coils.reserve (MODBUS_RTU_COIL_COUNT_MAX);
  discreteInputs.reserve (MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX);
  holdingRegisters.reserve (MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX);
  inputRegisters.reserve (MODBUS_RTU_INPUT_REGISTER_COUNT_MAX);
}

//======================================================================================//

String CSE_ModbusRTU_Server:: getName() {
  return name;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: begin() {
  return true;
}

//======================================================================================//
/**
 * @brief This function is used to poll the server for new requests. When a request is
 * received, it is diassembled into the request ADU and checked for validity. This
 * function takes care of checking what type of request it is and then send a response
 * back to the client. The response is assembled into the response ADU. Finally, the
 * type of request received is returned.
 * 
 * @return int 
 */
int CSE_ModbusRTU_Server:: poll() {
  // First received a new ADU from the client
  if (receive() < 0) {
    return -1;
  }

  // Now check if the address of the request matches the address of the server
  if (request.getDeviceAddress() != rtu->deviceAddress) {
    DEBUG_SERIAL.println (F("poll(): Server addresses does not match."));
    return -1;
  }

  // Check if the ADU received is an exception. A server is not meant to receive
  // a request that is an exception.
  if (request.getExceptionCode() != 0x00) {
    DEBUG_SERIAL.println (F("poll(): Received an exception request to server."));
    return -1;
  }

  // Now check what type of function code was received
  switch (request.getFunctionCode()) {
    case MODBUS_FC_READ_COILS: {
      // Check if the coil count is valid (the maximum in a request is 0x07D0) or
      // if all of the coils in the range are present in the server.
      if ((request.getQuantity() > 0x07D0) || (!isCoilPresent (request.getStartingAddress(), request.getQuantity()))) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_READ_COILS); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_VALUE); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_READ_COILS + 0x80; // Return exception function code
      }

      // If the request is valid, we can proceed with reading the coils specified
      // and adding them to the response ADU.
      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (MODBUS_FC_READ_COILS); // Set the function code of the response

      // Now we need to find how many bytes will be needed to pack the specified number of
      // coils states into the response ADU by packing each coil state into a bit.
      uint8_t byteCount = request.getQuantity() / 8; // Get the number of bytes needed

      if ((request.getQuantity() % 8) != 0) {
        byteCount++; // Add one more byte if the number of coils is not a multiple of 8
      }

      if (byteCount > 0xFB) {
        byteCount = 0xFB; // The maximum number of bytes is 0xFB
      }

      response.add (byteCount); // Set the byte count of the response

      // Now we need to pack the coil states into the response ADU
      uint8_t coilData [byteCount] = {0}; // Create an array to store the coil data

      for (int i = request.getStartingAddress(), j = 0; i < (request.getStartingAddress() + request.getQuantity()); i++) {
        if (coils [i].value == 0) {
          coilData [j / 8] &= ~(1U << (j % 8)); // Clear the bit
        }
        else {
          coilData [j / 8] |= (1U << (j % 8)); // Set the bit
        }
        j++;
      }

      // Now we need to copy the coil data into the response ADU
      response.add (coilData, byteCount); // Add the coil data to the response ADU
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return MODBUS_FC_READ_COILS; // Return the function code
      break;
    }
    
    //---------------------------------------------------------------------------------//

    case MODBUS_FC_READ_DISCRETE_INPUTS: {
      // Check if the discrete input count is valid (the maximum in a request is 0x07D0) or
      // if all of the discrete inputs in the range are present in the server.
      if ((request.getQuantity() > 0x07D0) || (!isDiscreteInputPresent (request.getStartingAddress(), request.getQuantity()))) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_READ_DISCRETE_INPUTS); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_VALUE); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_READ_DISCRETE_INPUTS + 0x80; // Return exception function code
      }

      // If the request is valid, we can proceed with reading the discrete inputs specified
      // and adding them to the response ADU.
      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (MODBUS_FC_READ_DISCRETE_INPUTS); // Set the function code of the response

      // Now we need to find how many bytes will be needed to pack the specified number of
      // discrete input states into the response ADU by packing each discrete input state into a bit.
      uint8_t byteCount = request.getQuantity() / 8; // Get the number of bytes needed

      if ((request.getQuantity() % 8) != 0) {
        byteCount++; // Add one more byte if the number of discrete inputs is not a multiple of 8
      }

      if (byteCount > 0xFB) {
        byteCount = 0xFB; // The maximum number of bytes is 0xFB
      }

      response.add (byteCount); // Set the byte count of the response

      // Now we need to pack the discrete input states into the response ADU
      uint8_t discreteInputData [byteCount] = {0}; // Create an array to store the discrete input data

      for (int i = request.getStartingAddress(), j = 0; i < (request.getStartingAddress() + request.getQuantity()); i++) {
        if (discreteInputs [i].value == 0) {
          discreteInputData [j / 8] &= ~(1U << (j % 8)); // Clear the bit
        }
        else {
          discreteInputData [j / 8] |= (1U << (j % 8)); // Set the bit
        }
        j++;
      }

      // Now we need to copy the discrete input data into the response ADU
      response.add (discreteInputData, byteCount); // Add the discrete input data to the response ADU
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return MODBUS_FC_READ_DISCRETE_INPUTS; // Return the function code
      break;
    }
    //---------------------------------------------------------------------------------//

    case MODBUS_FC_READ_HOLDING_REGISTERS: {
      // Check if the holding register count is valid (the maximum in a request is 0x007D) or
      // if all of the holding registers in the range are present in the server.
      if ((request.getQuantity() > 0x007D) || (!isHoldingRegisterPresent (request.getStartingAddress(), request.getQuantity()))) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_READ_HOLDING_REGISTERS); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_VALUE); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_READ_HOLDING_REGISTERS + 0x80; // Return exception function code
      }

      // If the request is valid, we can proceed with reading the holding registers specified
      // and adding them to the response ADU.
      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (MODBUS_FC_READ_HOLDING_REGISTERS); // Set the function code of the response

      uint16_t registerCount = request.getQuantity(); // Get the number of registers needed
      uint8_t byteCount = registerCount * 2; // Get the number of bytes needed

      uint8_t registerData [registerCount] = {0}; // Create an array to store the register data

      // Read the register data from the holding registers and write them to the array
      for (int i = request.getStartingAddress(), j = 0; i < (request.getStartingAddress() + request.getQuantity()); i++) {
        registerData [j] = holdingRegisters [i].value >> 8; // Get the high byte
        registerData [j + 1] = holdingRegisters [i].value & 0xFF; // Get the low byte
        j += 2;
      }

      // Now we need to copy the register data into the response ADU
      response.add (registerData, byteCount); // Add the register data to the response ADU
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return MODBUS_FC_READ_HOLDING_REGISTERS; // Return the function code
      break;
    }

    //---------------------------------------------------------------------------------//

    case MODBUS_FC_READ_INPUT_REGISTERS: {
      // Check if the input register count is valid (the maximum in a request is 0x007D) or
      // if all of the input registers in the range are present in the server.
      if ((request.getQuantity() > 0x007D) || (!isInputRegisterPresent (request.getStartingAddress(), request.getQuantity()))) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_READ_INPUT_REGISTERS); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_VALUE); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_READ_INPUT_REGISTERS + 0x80; // Return exception function code
      }

      // If the request is valid, we can proceed with reading the input registers specified
      // and adding them to the response ADU.
      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (MODBUS_FC_READ_INPUT_REGISTERS); // Set the function code of the response

      uint8_t registerCount = request.getQuantity(); // Get the number of registers needed (1-125)
      uint8_t byteCount = registerCount * 2; // Get the number of bytes needed

      uint8_t inputRegisterData [registerCount] = {0}; // Create an array to store the input register data

      // Read the register data from the input registers and write them to the array
      for (int i = request.getStartingAddress(), j = 0; i < (request.getStartingAddress() + request.getQuantity()); i++) {
        inputRegisterData [j] = inputRegisters [i].value >> 8; // Get the high byte
        inputRegisterData [j + 1] = inputRegisters [i].value & 0xFF; // Get the low byte
        j += 2;
      }

      // Now we need to copy the input register data into the response ADU
      response.add (inputRegisterData, byteCount); // Add the input register data to the response ADU
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return MODBUS_FC_READ_INPUT_REGISTERS; // Return the function code
      break;
    }

    //---------------------------------------------------------------------------------//

    case MODBUS_FC_WRITE_SINGLE_COIL: {
      // Check if the coil is present in the server
      if (!isCoilPresent (request.getStartingAddress())) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_WRITE_SINGLE_COIL); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_ADDRESS); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_WRITE_SINGLE_COIL + 0x80; // Return exception function code
      }

      DEBUG_SERIAL.print (F("poll(): Received request to write single coil 0x"));
      DEBUG_SERIAL.println (request.getStartingAddress(), HEX);

      // If the coil is present in the server, we can proceed with writing the coil specified.
      // The coil state will be after the starting address in the request ADU.
      // The state can be either 0x0000 (OFF) or 0xFF00 (ON).
      DEBUG_SERIAL.print (F("receive(): Writing value 0x"));
      if (request.getWord (MODBUS_RTU_ADU_DATA_INDEX + 2) == 0x00) {
        DEBUG_SERIAL.println (F("00"));
        writeCoil (request.getStartingAddress(), 0x00); // Write the coil to the server
      }
      else {
        DEBUG_SERIAL.println (F("01"));
        writeCoil (request.getStartingAddress(), 0x01); // Write the coil to the server
      }

      // For successful coil writes, the response ADU is the same as the request ADU
      response = request; // Copy the request ADU to the response ADU
      send(); // Send the response
      return MODBUS_FC_WRITE_SINGLE_COIL; // Return the function code
      break;
    }
    
    //---------------------------------------------------------------------------------//

    case MODBUS_FC_WRITE_SINGLE_REGISTER: {
      // Check if the holding register is present in the server
      if (!isHoldingRegisterPresent (request.getStartingAddress())) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_WRITE_SINGLE_REGISTER); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_ADDRESS); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_WRITE_SINGLE_REGISTER + 0x80; // Return exception function code
      }

      // If the holding register is present in the server, we can proceed with writing the holding register specified.
      // The holding register value will be after the starting address in the request ADU.
      writeHoldingRegister (request.getStartingAddress(), request.getWord (MODBUS_RTU_ADU_DATA_INDEX + 2)); // Write the holding register to the server

      // For successful holding register writes, the response ADU is the same as the request ADU
      response = request; // Copy the request ADU to the response ADU
      send(); // Send the response
      return MODBUS_FC_WRITE_SINGLE_REGISTER; // Return the function code
      break;
    }

    //---------------------------------------------------------------------------------//

    case MODBUS_FC_WRITE_MULTIPLE_COILS: {
      // Check if the coils are present in the server,
      // and the requested register count. The maximum register count is 0x07B0 (1968).
      if ((request.getQuantity() > 0x07B0) || (!isCoilPresent (request.getStartingAddress(), request.getQuantity()))) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_WRITE_MULTIPLE_COILS); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_ADDRESS); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_WRITE_MULTIPLE_COILS + 0x80; // Return exception function code
      }

      // The coil data will come packed as bits in the data field of the ADU.
      // So we need to extract each coil data and write them to the server.

      // Create an array with the sepcified number of coil registers.
      // Coil register count is not the same as the byte count.
      uint8_t coilData [request.getQuantity()] = {0}; //
      uint8_t byteCount = request.getByte (MODBUS_RTU_ADU_DATA_INDEX + 4); // Get the byte count

      // Now we need to copy the coil data from the request ADU
      for (int i, j = 0; i < byteCount; i++) {
        for (int k = 0; ((k < 8) && (j < request.getQuantity())); k++) {
          coilData [j] = (request.getByte (MODBUS_RTU_ADU_DATA_INDEX + 5 + i) >> k) & 0x01;
          j++;
        }
      }

      for (int i = 0; i < request.getQuantity(); i++) {
        writeCoil (request.getStartingAddress() + i, coilData [i]); // Write the coil to the server
      }

      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (MODBUS_FC_WRITE_MULTIPLE_COILS); // Set the function code of the response
      response.add (request.getStartingAddress()); // Set the starting address of the response
      response.add (request.getQuantity()); // Set the quantity of the response
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return MODBUS_FC_WRITE_MULTIPLE_COILS; // Return the function code
      break;
    }

    //---------------------------------------------------------------------------------//

    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS: {
      // Check if the holding registers are present in the server,
      // and the requested register count. The maximum register count is 0x007B (123).
      if ((request.getQuantity() > 0x007B) || (!isHoldingRegisterPresent (request.getStartingAddress(), request.getQuantity()))) {
        // Then process an exception
        response.resetLength(); // Reset the response length
        response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
        response.setFunctionCode (MODBUS_FC_WRITE_MULTIPLE_REGISTERS); // Set the function code of the response
        response.setException(); // Set the exception bit of the function code
        response.setExceptionCode (MODBUS_EX_ILLEGAL_DATA_ADDRESS); // Set the exception code
        response.setCRC(); // Set the CRC of the response
        send(); // Send the response
        return MODBUS_FC_WRITE_MULTIPLE_REGISTERS + 0x80; // Return exception function code
      }

      // The holding register data will come packed as 16-bit words in the data field of the ADU.
      // So we need to extract each holding register data and write them to the server.

      // Create an array with the sepcified number of holding registers.
      // Holding register count is not the same as the word count.
      uint16_t holdingRegisterData [request.getQuantity()] = {0}; //
      uint8_t byteCount = request.getByte (MODBUS_RTU_ADU_DATA_INDEX + 4); // Get the byte count

      // Now we need to copy the holding register data from the request ADU
      for (int i, j = 0; i < byteCount; i += 2) {
        holdingRegisterData [j] = request.getWord (MODBUS_RTU_ADU_DATA_INDEX + 5 + i);
        j++;
      }

      for (int i = 0; i < request.getQuantity(); i++) {
        writeHoldingRegister (request.getStartingAddress() + i, holdingRegisterData [i]); // Write the holding register to the server
      }

      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (MODBUS_FC_WRITE_MULTIPLE_REGISTERS); // Set the function code of the response
      response.add (request.getStartingAddress()); // Set the starting address of the response
      response.add (request.getQuantity()); // Set the quantity of the response
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return MODBUS_FC_WRITE_MULTIPLE_REGISTERS; // Return the function code
      break;
    }

    //---------------------------------------------------------------------------------//
    
    default: {
      DEBUG_SERIAL.print (F("poll(): Unsupported function code: 0x"));
      DEBUG_SERIAL.println (request.getFunctionCode(), HEX);
      DEBUG_SERIAL.println (F("poll(): Returning exception."));

      // Any unsupported function code will be processed as an exception
      response.resetLength(); // Reset the response length
      response.setDeviceAddress (rtu->deviceAddress); // Set the address of the response
      response.setFunctionCode (request.getFunctionCode()); // Set the function code of the response
      response.setException(); // Set the exception bit of the function code
      response.setExceptionCode (MODBUS_EX_ILLEGAL_FUNCTION); // Set the exception code
      response.setCRC(); // Set the CRC of the response
      send(); // Send the response
      return request.getFunctionCode() + 0x80; // Return exception function code
      break;
    }
  }
}

//======================================================================================//

int CSE_ModbusRTU_Server:: receive() {
  // A server will use the request ADU to receive requests from the client.
  return rtu->receive (request);
}

//======================================================================================//

int CSE_ModbusRTU_Server:: send() {
  // A server will use the response ADU to send responses to the client.
  return rtu->send (response);
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: configureCoils (uint16_t startAddress, uint16_t quantity) {
  // First check if the current coil count doesn't exceed the maximum coil count
  if (coils.size() == MODBUS_RTU_COIL_COUNT_MAX) {
    return false;
  }

  // Now check if the input coil count won's exceed the maximum coil count
  if ((coils.size() + quantity) > MODBUS_RTU_COIL_COUNT_MAX) {
    return false;
  }

  // Now we can add a new coil
  for (uint16_t i = 0; i < quantity; i++) {
    coils.push_back (modbus_bit_t (startAddress + i, 0x00));
  }

  return true;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: configureDiscreteInputs (uint16_t startAddress, uint16_t quantity) {
  // First check if the current discrete input count doesn't exceed the maximum discrete input count
  if (discreteInputs.size() == MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX) {
    return false;
  }

  // Now check if the input discrete input count won's exceed the maximum discrete input count
  if ((discreteInputs.size() + quantity) > MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX) {
    return false;
  }

  // Now we can add a new discrete input
  for (uint16_t i = 0; i < quantity; i++) {
    modbus_bit_t* newCoil = new modbus_bit_t (startAddress + i, 0x00);
    discreteInputs.push_back (*newCoil);
  }
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: configureInputRegisters (uint16_t startAddress, uint16_t quantity) {
  // First check if the current input register count doesn't exceed the maximum input register count
  if (inputRegisters.size() == MODBUS_RTU_INPUT_REGISTER_COUNT_MAX) {
    return false;
  }

  // Now check if the input input register count won's exceed the maximum input register count
  if ((inputRegisters.size() + quantity) > MODBUS_RTU_INPUT_REGISTER_COUNT_MAX) {
    return false;
  }

  // Now we can add a new input register
  for (uint16_t i = 0; i < quantity; i++) {
    inputRegisters.push_back (modbus_register_t (startAddress + i, 0x0000));
  }

  return true;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: configureHoldingRegisters (uint16_t startAddress, uint16_t quantity) {
  // First check if the current holding register count doesn't exceed the maximum holding register count
  if (holdingRegisters.size() == MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX) {
    return false;
  }

  // Now check if the input holding register count won's exceed the maximum holding register count
  if ((holdingRegisters.size() + quantity) > MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX) {
    return false;
  }

  // Now we can add a new holding register
  for (uint16_t i = 0; i < quantity; i++) {
    holdingRegisters.push_back (modbus_register_t (startAddress + i, 0x0000));
  }
}

//======================================================================================//

int CSE_ModbusRTU_Server:: readCoil (uint16_t address) {
  // First check if the coil exists
  for (uint16_t i = 0; i < coils.size(); i++) {
    if (coils [i].address == address) {
      return coils [i].value;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeCoil (uint16_t address, uint8_t value) {
  if ((value != 0x00) && (value != 0x01)) {
    return -1;
  }

  // First check if the coil exists
  for (uint16_t i = 0; i < coils.size(); i++) {
    if (coils [i].address == address) {
      coils [i].value = value;
      return 1;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeCoil (uint16_t address, uint8_t value, uint16_t count) {
  if ((value != 0x00) && (value != 0x01)) {
    return -1;
  }

  // Check if the coil addresses go out of bounds
  if ((address + count) > (coils [coils.size() - 1].address + 1)) {
    return -1;
  }

  int result = 1; // Assume success

  // Write the specified coils
  for (uint16_t i = 0; i < count; i++) {
    if (writeCoil (address + i, value) == -1) { // Check if any of the writes fail
      result = -1;  // If so, set the result to failure
    }
  }

  return result;
}

//======================================================================================//
/**
 * @brief Checks if a single coil with address is present in the server. The coil
 * addresses don't have to be contiguous. But ideally it should be.
 * 
 * @param address 
 * @return true 
 * @return false 
 */
bool CSE_ModbusRTU_Server:: isCoilPresent (uint16_t address) {
  // Check if the coil exists
  for (uint16_t i = 0; i < coils.size(); i++) {
    if (coils [i].address == address) {
      return true;
    }
  }

  return false;
}

//======================================================================================//
/**
 * @brief Checks if a range of coils is present. If any of the coils is not present
 * in the server, the function returns false. The coil addresses don't have to be
 * contiguous. But ideally it should be. For example, it is possile to have coils with
 * addresses 0x01, 0x03, 0x04, 0x07, etc. These are not contigous addresses. So we
 * need to compare each of the coils specified in the range with the coils present
 * in the server.
 * 
 * @param address 
 * @param count 
 * @return true 
 * @return false 
 */
bool CSE_ModbusRTU_Server:: isCoilPresent (uint16_t address, uint16_t count) {
  // // Check if the coil addresses go out of bounds
  // if ((address + count) > (coils [coils.size() - 1].address + 1)) {
  //   return false;
  // }

  // Check if each of the coils exist
  for (uint16_t i = 0; i < count; i++) {
    if (!isCoilPresent (address + i)) { // If any of the coils don't exist, return false
      return false;
    }
  }

  return true;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: readDiscreteInput (uint16_t address) {
  // First check if the discrete input exists
  for (uint16_t i = 0; i < discreteInputs.size(); i++) {
    if (discreteInputs [i].address == address) {
      if ((discreteInputs [i].value == 0x00) || (discreteInputs [i].value == 0x01)) {
        return discreteInputs [i].value;
      }
    }
  }

  return -1;
}

//======================================================================================//
/**
 * @brief Writes the specified discrete input. Even though discrete inputs are read-
 * only, a server can write its own discrete inputs to update their states.
 * 
 * @param address 
 * @param value 
 * @return int 
 */
int CSE_ModbusRTU_Server:: writeDiscreteInput (uint16_t address, uint8_t value) {
  if ((value != 0x00) && (value != 0x01)) {
    return -1;
  }

  // First check if the discrete input exists
  for (uint16_t i = 0; i < discreteInputs.size(); i++) {
    if (discreteInputs [i].address == address) {
      discreteInputs [i].value = value;
      return 1;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeDiscreteInput (uint16_t address, uint8_t value, uint16_t count) {
  if ((value != 0x00) && (value != 0x01)) {
    return -1;
  }

  // Check if the discrete input addresses go out of bounds
  if ((address + count) > (discreteInputs [discreteInputs.size() - 1].address + 1)) {
    return -1;
  }

  int result = 1; // Assume success

  // Write the specified discrete inputs
  for (uint16_t i = 0; i < count; i++) {
    if (writeDiscreteInput (address + i, value) == -1) { // Check if any of the writes fail
      result = -1; // If so, set the result to failure
    }
  }

  return result;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: isDiscreteInputPresent (uint16_t address) {
  // Check if the discrete input exists
  for (uint16_t i = 0; i < discreteInputs.size(); i++) {
    if (discreteInputs [i].address == address) {
      return true;
    }
  }

  return false;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: isDiscreteInputPresent (uint16_t address, uint16_t count) {
  // Check if each of the discrete inputs exist
  for (uint16_t i = 0; i < count; i++) {
    if (!isDiscreteInputPresent (address + i)) { // If any of the discrete inputs don't exist, return false
      return false;
    }
  }

  return true;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: readInputRegister (uint16_t address) {
  // First check if the input register exists
  for (uint16_t i = 0; i < inputRegisters.size(); i++) {
    if (inputRegisters [i].address == address) {
      return inputRegisters [i].value;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeInputRegister (uint16_t address, uint16_t value) {
  // First check if the input register exists
  for (uint16_t i = 0; i < inputRegisters.size(); i++) {
    if (inputRegisters [i].address == address) {
      inputRegisters [i].value = value;
      return 1;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeInputRegister (uint16_t address, uint16_t value, uint16_t count) {
  // Check if the input register addresses go out of bounds
  if ((address + count) > (inputRegisters [inputRegisters.size() - 1].address + 1)) {
    return -1;
  }

  int result = 1; // Assume success

  // Write the specified input registers
  for (uint16_t i = 0; i < count; i++) {
    if (writeInputRegister (address + i, value) == -1) { // Check if any of the writes fail
      result = -1; // If so, set the result to failure
    }
  }

  return result;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: isInputRegisterPresent (uint16_t address) {
  // Check if the input register exists
  for (uint16_t i = 0; i < inputRegisters.size(); i++) {
    if (inputRegisters [i].address == address) {
      return true;
    }
  }

  return false;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: isInputRegisterPresent (uint16_t address, uint16_t count) {
  // Check if each of the input registers exist
  for (uint16_t i = 0; i < count; i++) {
    if (!isInputRegisterPresent (address + i)) { // If any of the input registers don't exist, return false
      return false;
    }
  }

  return true;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: readHoldingRegister (uint16_t address) {
  // First check if the holding register exists
  for (uint16_t i = 0; i < holdingRegisters.size(); i++) {
    if (holdingRegisters [i].address == address) {
      return holdingRegisters [i].value;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeHoldingRegister (uint16_t address, uint16_t value) {
  // First check if the holding register exists
  for (uint16_t i = 0; i < holdingRegisters.size(); i++) {
    if (holdingRegisters [i].address == address) {
      holdingRegisters [i].value = value;
      return 1;
    }
  }

  return -1;
}

//======================================================================================//

int CSE_ModbusRTU_Server:: writeHoldingRegister (uint16_t address, uint16_t value, uint16_t count) {
  // Check if the holding register addresses go out of bounds
  if ((address + count) > (holdingRegisters [holdingRegisters.size() - 1].address + 1)) {
    return -1;
  }

  int result = 1; // Assume success

  // Write the specified holding registers
  for (uint16_t i = 0; i < count; i++) {
    if (writeHoldingRegister (address + i, value) == -1) { // Check if any of the writes fail
      result = -1; // If so, set the result to failure
    }
  }

  return result;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: isHoldingRegisterPresent (uint16_t address) {
  // Check if the holding register exists
  for (uint16_t i = 0; i < holdingRegisters.size(); i++) {
    if (holdingRegisters [i].address == address) {
      return true;
    }
  }

  return false;
}

//======================================================================================//

bool CSE_ModbusRTU_Server:: isHoldingRegisterPresent (uint16_t address, uint16_t count) {
  // Check if each of the holding registers exist
  for (uint16_t i = 0; i < count; i++) {
    if (!isHoldingRegisterPresent (address + i)) { // If any of the holding registers don't exist, return false
      return false;
    }
  }

  return true;
}

//======================================================================================//

