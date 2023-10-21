
//======================================================================================//
/**
 * @file CSE_ModbusRTU.cpp
 * @brief Main source file for the CSE_ModbusRTU library.
 * @date +05:30 01:05:47 PM 21-10-2023, Saturday
 * @version 0.0.6
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
 * Resetting the aduLength will prevent you from doing any operation on the buffer even
 * if it has valid data. So use this function with caution, and only before you start
 * writing to the buffer.
 * 
 * @return true - Operation successful.
 * @return false - Operation failed.
 */
bool CSE_ModbusRTU_ADU:: resetLength() {
  aduLength = 0;
  return true;
}

//======================================================================================//
/**
 * @brief Returns the number of valid bytes in the ADU buffer. A value 0 indicates that
 * the buffer has no valid data even though its contents may not be all 0x00. The aduLength
 * is a very important variable. It is used to keep track of the number of valid bytes in
 * ADU buffer. If you change the aduLength manually, you will end up with a corrupted ADU
 * buffer
 * 
 * @return uint8_t - The current aduLength.
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
 * @brief Add a single byte to the ADU buffer. The new byte is written at the end of the
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
 * @brief Add a word (16-bit) to the ADU buffer. The new word is written to the end of
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

  // The Hi byte is added first
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
 * @param length The number of 16-bit data words to add (not the number of bytes)
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
 * @brief Calculates the CRC of the ADU and compares it to the CRC in the ADU. If the ADU
 * length is less than 3, that means that the device address, function code, and data are
 * not set yet. In this case, we can't calculate the CRC and the function returns `false`.
 * 
 * @return true - If the CRCs match.
 * @return false - If the CRCs do not match.
 */
bool CSE_ModbusRTU_ADU:: checkCRC() {
  // If the ADU length is less than 3, that means that the device address, function code,
  // and data are not set yet. In this case, we can't calculate the CRC.
  if (aduLength < 3) {
    MODBUS_DEBUG_SERIAL.println (F("checkCRC(): Error - ADU length is less than 3."));
    return false;
  }

  // To check CRC, it must have been already set.
  uint16_t crc = calculateCRC (true);

  if ((aduBuffer [aduLength - 2] == (uint8_t) (crc & 0xFF)) && (aduBuffer [aduLength - 1] == (uint8_t) (crc >> 8))) {
    // MODBUS_DEBUG_SERIAL.println (F("checkCRC(): CRCs match."));
    return true;
  }
  else {
    MODBUS_DEBUG_SERIAL.print (F("checkCRC(): Error - CRCs do not match. Found: 0x"));
    MODBUS_DEBUG_SERIAL.print (aduBuffer [aduLength - 2], HEX);
    MODBUS_DEBUG_SERIAL.print (aduBuffer [aduLength - 1], HEX);
    MODBUS_DEBUG_SERIAL.print (F(", Calculated: 0x"));
    MODBUS_DEBUG_SERIAL.println (crc, HEX);
    return false;
  }
}

//======================================================================================//
/**
 * @brief Calculates the CRC of the ADU contents and returns it. If the aduLength is
 * not sufficient to calculate the CRC then 0x00 is returned. The CRC can be calculated
 * for the entire ADU buffer, or exclude the last two bytes if the CRC is already set.
 * 
 * @param isCRCSet If true, the CRC is already set and should not be included in the
 * CRC calculation. If false, the CRC is not set and use the entire ADU bufer for CRC
 * calculation.
 * @return uint16_t - The CRC of the ADU contents.
 */
uint16_t CSE_ModbusRTU_ADU:: calculateCRC (bool isCRCSet) {
  // If the ADU length is less than 3, that means that the device address, function code,
  // and data are not set yet. In this case, we can't calculate the CRC.
  if (aduLength < 3) {
    return 0x0000;
  }

  uint16_t crc = 0xFFFF;

  uint8_t length = 0;

  if (isCRCSet) {
    // If the CRC is already set, then we don't want to include it in the CRC calculation.
    length = aduLength - MODBUS_RTU_CRC_LENGTH;
  } else {
    // If CRC is not set, we can calculate the CRC over the entire ADU buffer.
    length = aduLength;
  }

  for (uint8_t i = 0; i < length; i++) {
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
 * @param type A valid ADU type. Check `aduType_t`.
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
 * @param address Client or server address (8-bit)
 * @return true - Operation successful.
 * @return false - Operation failed.
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
 * @param functionCode Valid Modbus function code from 0x01 to 0x7F (1 - 127)
 * @return true - Operation successful.
 * @return false - Operation failed.
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
  // This is because the exception code always comes after the function code.
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
 * @return true - Operation successful.
 * @return false - Operation failed.
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

  // Start writing from after the function code field.
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
    MODBUS_DEBUG_SERIAL.println (F("setCRC(): ADU length is less than 3. Can't set CRC."));
    return 0x0000;
  }

  uint16_t crc = calculateCRC (false);  // Use the entire buffer data.

  // CRC is written with Lo byte first, unlike the data field.
  aduBuffer [aduLength++] = (uint8_t) (crc & 0xFF); // Low byte
  aduBuffer [aduLength++] = (uint8_t) (crc >> 8); // High byte

  MODBUS_DEBUG_SERIAL.print (F("setCRC(): CRC is 0x"));
  MODBUS_DEBUG_SERIAL.println (crc, HEX);

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
 * @brief Returns the exception code from the ADU. In a valid exception ADU, the code
 * comes just after the function code. Only works for ADU  with the type set to
 * EXCEPTION or the exception bit in the function code is set . So make sure you check
 * the type before calling this function.
 * 
 * @return uint8_t - The exception code. 0x00 if no valid exception code.
 */
uint8_t CSE_ModbusRTU_ADU:: getExceptionCode() {
  if ((aduType == aduType_t:: EXCEPTION) || (aduBuffer [MODBUS_RTU_ADU_FUNCTION_CODE_INDEX] > 0x80)) { // Check if the ADU is an exception
    return aduBuffer [MODBUS_RTU_ADU_DATA_INDEX];
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
 * @brief Returns the quantity field in the ADU. The quantity is the register count in
 * most cases. The function only reads the value at the position, and does not guarantee
 * that the value is valid.
 * 
 * @return uint16_t - The quantity (register count) in the ADU.
 */
uint16_t CSE_ModbusRTU_ADU:: getQuantity() {
  // Quantity always comes after the starting address in the data.
  return getWord (MODBUS_RTU_ADU_DATA_INDEX + 2);
}

//======================================================================================//
/**
 * @brief Returns the CRC field from the ADU. Operation fails if the ADU length is less
 * than 4.
 * 
 * @return uint16_t - CRC value.
 */
uint16_t CSE_ModbusRTU_ADU:: getCRC() {
  if (aduLength < 4) {
    return 0x0000;
  }

  return (uint16_t) (aduBuffer [aduLength - 2] << 8) + aduBuffer [aduLength - 1];
}

//======================================================================================//
/**
 * @brief Returns the length of data field in the ADU. This is the number of bytes
 * after the function code and before the CRC. The length is calculated purely from
 * the aduLength. So if the aduLength is not correct for some reason, the function
 * will return an incorrect value. The ADU length should be at least 5 bytes.
 * 
 * @return uint8_t - The length of data field in the ADU.
 */
uint8_t CSE_ModbusRTU_ADU:: getDataLength() {
  if (aduLength < 5) {
    return 0x00;
  }
  // Return the number of bytes in the ADU after device address, function code and CRC
  return (aduLength - MODBUS_RTU_ADU_DATA_INDEX - MODBUS_RTU_CRC_LENGTH); // aduLength - 4
}

//======================================================================================//
/**
 * @brief Returns a single byte from the ADU. The index should be within the aduLength.
 * If the index is not valid (greater than aduLength), the function returns 0x00. So
 * this does not guarantee that the function will always return a valid byte.
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
 * returns 0x0000. So this does not guarantee that the function will always return a valid
 * word. Two consecutive bytes are combined to form the word. The index indicates the
 * position of the Hi byte. The operation fails if the ADU length is less than 2.
 * 
 * @param index The starting index of the word to return.
 * @return uint16_t - The 16-bit word at the specified index.
 */
uint16_t CSE_ModbusRTU_ADU:: getWord (uint8_t index) {
  if (aduLength < 2) {
    return 0x0000;
  }

  if ((index + 1) < aduLength) {
    return (uint16_t) (aduBuffer [index] << 8) + aduBuffer [index + 1];
  }
  else {
    return 0x0000;
  }
}

//======================================================================================//
/**
 * @brief Returns the current type of the ADU. The ADU type is converted to an integer.
 * 
 * @return int - The ADU type as an integer.
 */
int CSE_ModbusRTU_ADU:: getType() {
  return aduType;
}

//======================================================================================//
/**
 * @brief Prints the ADU as a hex string.
 * 
 */
void CSE_ModbusRTU_ADU:: print() {
  // Print the ADU as a hex string
  MODBUS_DEBUG_SERIAL.print ("ADU: ");

  for (uint8_t i = 0; i < aduLength; i++) {
    if (aduBuffer [i] < 0x10) {
      MODBUS_DEBUG_SERIAL.print ("0x0");
    }
    else {
      MODBUS_DEBUG_SERIAL.print ("0x");
    }
    
    MODBUS_DEBUG_SERIAL.print (aduBuffer [i], HEX);
    MODBUS_DEBUG_SERIAL.print (" ");
  }

  MODBUS_DEBUG_SERIAL.println();
}

//======================================================================================//
/**
 * @brief Instantiate a new CSE_ModbusRTU object. The device address is the host
 * address. Serial port can be hardware or software serial port if you are using the
 * CSE_ArduinoRS485 library.
 * 
 * @param serialPort The serial port to read/write data.
 * @param deviceAddress The 8-bit device address.
 * @param name The name of the Modbus RTU object.
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
 * @param client CSE_ModbusRTU_Client object.
 * @return true - Operation successful.
 * @return false - Operation failed.
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
 * @param adu The ADU object to save the incoming data.
 * @return int - The ADU length, or -1 if the operation fails.
 */
int CSE_ModbusRTU:: receive (CSE_ModbusRTU_ADU& adu, uint32_t timeout) {
  adu.resetLength(); // Reset the ADU length
  // MODBUS_DEBUG_SERIAL.print (F("receive(): Checking Modbus port.."));
  
  uint32_t startTime = millis();

  while ((millis() - startTime) < timeout) {
    // Read all the bytes from the serial port
    while ((serialPort->available() > 0) && (adu.getLength() < MODBUS_RTU_ADU_LENGTH_MAX)) {
      adu.add ((uint8_t) serialPort->read()); // Read the first byte
    }
  }

  // Print the ADU
  if (adu.getLength() > 0) {
    MODBUS_DEBUG_SERIAL.print (F("receive(): Received ADU:"));
    for (int i = 0; i < adu.getLength(); i++) {
      MODBUS_DEBUG_SERIAL.print (" ");
      if (adu.getByte (i) < 0x10) {
        MODBUS_DEBUG_SERIAL.print (F("0x0"));
      }
      else {
        MODBUS_DEBUG_SERIAL.print (F("0x"));
      }
      MODBUS_DEBUG_SERIAL.print (adu.getByte (i), HEX);
    }

    MODBUS_DEBUG_SERIAL.println();
  }

  // Now check if the ADU is valid. We can do this by simply checking the CRC of the ADU.
  if (adu.getLength() > 0) {
    if (adu.checkCRC()) { // Check the CRC of the ADU
      MODBUS_DEBUG_SERIAL.println (F("receive(): ADU CRC passed"));
      return (int) adu.getLength(); // Return the length of the ADU
    }
    else {
      MODBUS_DEBUG_SERIAL.println (F("receive(): ADU CRC failed"));
    }
  }
  
  return -1;
}

//======================================================================================//
/**
 * @brief Sends the specified ADU to the serial port. The function will check the CRC
 * before sending it. Returns the ADU length if the operation was successful. Otherwise
 * it will return -1.
 * 
 * @param adu The ADU to send.
 * @return int - ADU length, or -1 if the operation fails.
 */
int CSE_ModbusRTU:: send (CSE_ModbusRTU_ADU& adu) {
  // Check if the ADU is valid
  if (adu.checkCRC()) {
    MODBUS_DEBUG_SERIAL.print (F("send(): Sending ADU:"));

    // Print the ADU
    for (int i = 0; i < adu.getLength(); i++) {
      MODBUS_DEBUG_SERIAL.print (" ");
      if (adu.getByte (i) < 0x10) {
        MODBUS_DEBUG_SERIAL.print (F("0x0"));
      }
      else {
        MODBUS_DEBUG_SERIAL.print (F("0x"));
      }
      MODBUS_DEBUG_SERIAL.print (adu.getByte (i), HEX);
    }

    MODBUS_DEBUG_SERIAL.println();

    // Send the ADU
    serialPort->beginTransmission();
    
    for (uint8_t i = 0; i < adu.getLength(); i++) {
      serialPort->write (adu.getByte (i));
    }

    serialPort->endTransmission();

    return adu.getLength(); // Return the length of the ADU
  }

  MODBUS_DEBUG_SERIAL.println (F("send(): CRC checking failed!"));
  adu.print();

  return -1;
}

//======================================================================================//
/**
 * @brief Instantiates a new Modbus server object. You must send a parent Modbus RTU
 * object and a name for the server. The name is used for debugging purposes.
 * 
 * @param rtu Parent Modbus RTU object.
 * @param name Name of the server.
 * @return CSE_ModbusRTU_Server:: 
 */
CSE_ModbusRTU_Server:: CSE_ModbusRTU_Server (CSE_ModbusRTU& rtu, String name) {
  this->rtu = &rtu;
  this->name = name;

  // Set the default request and response ADU types
  request.setType (CSE_ModbusRTU_ADU::aduType_t:: REQUEST);
  response.setType (CSE_ModbusRTU_ADU::aduType_t:: RESPONSE);


  // Reserve memory for the data arrays. This is not necessary but it will prevent
  // memory fragmentation.
  coils.reserve (MODBUS_RTU_COIL_COUNT_MAX);
  discreteInputs.reserve (MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX);
  holdingRegisters.reserve (MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX);
  inputRegisters.reserve (MODBUS_RTU_INPUT_REGISTER_COUNT_MAX);
}

//======================================================================================//
/**
 * @brief Returns the name of the server.
 * 
 * @return String - Name of the server.
 */
String CSE_ModbusRTU_Server:: getName() {
  return name;
}

//======================================================================================//
/**
 * @brief Does nothing for now.
 * 
 * @return true 
 * @return false 
 */
bool CSE_ModbusRTU_Server:: begin() {
  return true;
}

//======================================================================================//
/**
 * @brief This function is used to poll the server for new requests. When a request is
 * received, it is disassembled into the request ADU and checked for validity. This
 * function takes care of checking what type of request it is and then send a response
 * back to the client. The response is assembled into the response ADU. Finally, the
 * type of request received is returned.
 * 
 * @return int - Function code, or -1 if the operation fails.
 */
int CSE_ModbusRTU_Server:: poll() {
  // First received a new ADU from the client
  if (receive() < 0) {
    return -1;
  }

  // Now check if the address of the request matches the address of the server
  if (request.getDeviceAddress() != rtu->deviceAddress) {
    MODBUS_DEBUG_SERIAL.println (F("poll(): Server addresses does not match."));
    return -1;
  }

  // Check if the ADU received is an exception. A server is not meant to receive
  // a request that is an exception.
  if (request.getExceptionCode() != 0x00) {
    MODBUS_DEBUG_SERIAL.println (F("poll(): Received an exception request to server."));
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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to read coils 0x"));
      MODBUS_DEBUG_SERIAL.print (request.getStartingAddress(), HEX);
      MODBUS_DEBUG_SERIAL.print (F(" to 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress() + request.getQuantity() - 1, HEX);

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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to read discrete inputs 0x"));
      MODBUS_DEBUG_SERIAL.print (request.getStartingAddress(), HEX);
      MODBUS_DEBUG_SERIAL.print (F(" to 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress() + request.getQuantity() - 1, HEX);


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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to read holding registers 0x"));
      MODBUS_DEBUG_SERIAL.print (request.getStartingAddress(), HEX);
      MODBUS_DEBUG_SERIAL.print (F(" to 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress() + request.getQuantity() - 1, HEX);


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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to read input registers 0x"));
      MODBUS_DEBUG_SERIAL.print (request.getStartingAddress(), HEX);
      MODBUS_DEBUG_SERIAL.print (F(" to 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress() + request.getQuantity() - 1, HEX);


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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to write single coil 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress(), HEX);

      // If the coil is present in the server, we can proceed with writing the coil specified.
      // The coil state will be after the starting address in the request ADU.
      // The state can be either 0x0000 (OFF) or 0xFF00 (ON).
      MODBUS_DEBUG_SERIAL.print (F("poll(): Writing value 0x"));
      if (request.getWord (MODBUS_RTU_ADU_DATA_INDEX + 2) == 0x00) {
        MODBUS_DEBUG_SERIAL.println (F("00"));
        writeCoil (request.getStartingAddress(), 0x00); // Write the coil to the server
      }
      else {
        MODBUS_DEBUG_SERIAL.println (F("01"));
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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to write single register 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress(), HEX);

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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to write multiple coils 0x"));
      MODBUS_DEBUG_SERIAL.print (request.getStartingAddress(), HEX);
      MODBUS_DEBUG_SERIAL.print (F(" to 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress() + request.getQuantity() - 1, HEX);

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

      MODBUS_DEBUG_SERIAL.print (F("poll(): Received request to write multiple registers 0x"));
      MODBUS_DEBUG_SERIAL.print (request.getStartingAddress(), HEX);
      MODBUS_DEBUG_SERIAL.print (F(" to 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getStartingAddress() + request.getQuantity() - 1, HEX);

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
      MODBUS_DEBUG_SERIAL.print (F("poll(): Received unsupported function code: 0x"));
      MODBUS_DEBUG_SERIAL.println (request.getFunctionCode(), HEX);
      MODBUS_DEBUG_SERIAL.println (F("poll(): Returning exception."));

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
/**
 * @brief Receives a request from the client. Provided to access receive functionality
 * through the server.
 * 
 * @return int - ADU length
 */
int CSE_ModbusRTU_Server:: receive() {
  // A server will use the request ADU to receive requests from the client.
  return rtu->receive (request);
}

//======================================================================================//
/**
 * @brief Sends a response to the client. Provided to access send functionality through
 * the server.
 * 
 * @return int 
 */
int CSE_ModbusRTU_Server:: send() {
  // A server will use the response ADU to send responses to the client.
  return rtu->send (response);
}

//======================================================================================//
/**
 * @brief Configures the coil data array by adding new data to the coils vector.
 * The maximum coil count is limited to MODBUS_RTU_COIL_COUNT_MAX which you can change
 * if required. You can create a contiguous set of coils by specifying the starting
 * address and the quantity of coils to be created. If you want coils of different
 * and non-contiguous addresses, you can call this function multiple times. This library
 * treats individual data as unique and independent; not as part of a contiguous set.
 * So two adjacent coils in the data array can have non-contiguous addresses.
 * 
 * @param startAddress The starting address of the coil (16-bit)
 * @param quantity The number of coils to create (16-bit)
 * @return true - Operation successful
 * @return false - Operation failed
 */
bool CSE_ModbusRTU_Server:: configureCoils (uint16_t startAddress, uint16_t quantity) {
  // First check if the current coil count doesn't exceed the maximum coil count
  if (coils.size() == MODBUS_RTU_COIL_COUNT_MAX) {
    return false;
  }

  // Now check if the input coil count won't exceed the maximum coil count
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
/**
 * @brief Configures the discrete input data array by adding new data to the discrete
 * inputs vector. The maximum discrete input count is limited to MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX
 * which you can change if required. You can create a contiguous set of discrete inputs
 * by specifying the starting address and the quantity of discrete inputs to be created.
 * If you want discrete inputs of different and non-contiguous addresses, you can call
 * this function multiple times. This library treats individual data as unique and
 * independent; not as part of a contiguous set. So two adjacent discrete inputs in the
 * data array can have non-contiguous addresses.
 * 
 * @param startAddress The starting address of the discrete input (16-bit)
 * @param quantity The number of discrete inputs to create (16-bit)
 * @return true - Operation successful
 * @return false - Operation failed
 */
bool CSE_ModbusRTU_Server:: configureDiscreteInputs (uint16_t startAddress, uint16_t quantity) {
  // First check if the current discrete input count doesn't exceed the maximum discrete input count
  if (discreteInputs.size() == MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX) {
    return false;
  }

  // Now check if the input discrete input count won't exceed the maximum discrete input count
  if ((discreteInputs.size() + quantity) > MODBUS_RTU_DISCRETE_INPUT_COUNT_MAX) {
    return false;
  }

  // Now we can add a new discrete input
  for (uint16_t i = 0; i < quantity; i++) {
    modbus_bit_t* newCoil = new modbus_bit_t (startAddress + i, 0x00);
    discreteInputs.push_back (*newCoil);
  }

  return true;
}

//======================================================================================//
/**
 * @brief Configures the input register data array by adding new data to the input
 * registers vector. The maximum input register count is limited to MODBUS_RTU_INPUT_REGISTER_COUNT_MAX
 * which you can change if required. You can create a contiguous set of input registers
 * by specifying the starting address and the quantity of input registers to be created.
 * If you want input registers of different and non-contiguous addresses, you can call
 * this function multiple times. This library treats individual data as unique and
 * independent; not as part of a contiguous set. So two adjacent input registers in the
 * data array can have non-contiguous addresses.
 * 
 * @param startAddress The starting address of the input register (16-bit)
 * @param quantity The number of input registers to create (16-bit)
 * @return true - Operation successful
 * @return false - Operation failed
 */
bool CSE_ModbusRTU_Server:: configureInputRegisters (uint16_t startAddress, uint16_t quantity) {
  // First check if the current input register count doesn't exceed the maximum input register count
  if (inputRegisters.size() == MODBUS_RTU_INPUT_REGISTER_COUNT_MAX) {
    return false;
  }

  // Now check if the input input register count won't exceed the maximum input register count
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
/**
 * @brief Configures the holding register data array by adding new data to the holding
 * registers vector. The maximum holding register count is limited to MODBUS_RTU_HOLDING_REGISTER_COUNT_MAX
 * which you can change if required. You can create a contiguous set of holding registers
 * by specifying the starting address and the quantity of holding registers to be created.
 * If you want holding registers of different and non-contiguous addresses, you can call
 * this function multiple times. This library treats individual data as unique and
 * independent; not as part of a contiguous set. So two adjacent holding registers in the
 * data array can have non-contiguous addresses.
 * 
 * @param startAddress The starting address of the holding register (16-bit)
 * @param quantity The number of holding registers to create (16-bit)
 * @return true - Operation successful
 * @return false - Operation failed
 */
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

  return true;
}

//======================================================================================//
/**
 * @brief Reads a single coil from the coil data array. The address is checked for validity
 * by comparing it with the coil addresses in the coil data array. If the address is valid,
 * the coil value is returned. If the address is invalid, -1 is returned.
 * 
 * @param address - The 16-bit address of the coil.
 * @return int - Coil value; -1 if address is invalid.
 */
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
/**
 * @brief Writes a single coil register on the server. The address is checked for validity
 * by comparing it with the coil addresses in the coil data array. If the address is valid,
 * the coil value is updated and 1 is returned. If the address is invalid, -1 is returned.
 * The coil value can be 0x00 or 0x01. Any other value is invalid.
 * 
 * This function does not send anything to the client. This function is only used to update
 * the server-side data.
 * 
 * @param address The 16-bit address of the coil.
 * @param value 0x00 for OFF, and 0x01 for ON.
 * @return int - 1 if successful; -1 if failed.
 */
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
/**
 * @brief Writes a contiguous series of coil registers on the server. Individual addresses
 * are checked for validity by comparing them with the coil addresses in the coil data array.
 * If any of the addresses are invalid, the function stops writing the values and returns 
 * -1. If all the addresses are valid, the coil values are updated and 1 is returned.
 * The coil value can be 0x00 or 0x01. Any other value is invalid.
 * 
 * @param address The 16-bit starting address of the coils.
 * @param value 0x00 for OFF, and 0x01 for ON.
 * @param count The number of coils to write.
 * @return int - 1 if successful; -1 if failed.
 */
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
 * @param address The coil address to check.
 * @return true - Coil is present in the data array.
 * @return false - Coil is not found.
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
 * @param address The 16-bit starting address of the coils.
 * @param count The number of coils to check.
 * @return true - All coils are present in the server.
 * @return false - One or more coils are not present in the server.
 */
bool CSE_ModbusRTU_Server:: isCoilPresent (uint16_t address, uint16_t count) {
  // Check if each of the coils exist
  for (uint16_t i = 0; i < count; i++) {
    if (!isCoilPresent (address + i)) { // If any of the coils don't exist, return false
      return false;
    }
  }

  return true;
}

//======================================================================================//
/**
 * @brief Reads a single discrete input register on the server. The address is checked
 * for validity.
 * 
 * @param address The 16-bit address of the discrete input register.
 * @return int - 0x00 if OFF; 0x01 if ON; -1 if failed.
 */
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
 * @param address The 16-bit address of the discrete input register.
 * @param value 0x00 for OFF, and 0x01 for ON.
 * @return int - 1 if successful; -1 if failed.
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
/**
 * @brief Writes a contiguous series of discrete input registers on the server. Individual
 * addresses are checked for validity by comparing them with the discrete input addresses
 * in the discrete input data array. If any of the addresses are invalid, the function
 * stops writing the values and returns -1. If all the addresses are valid, the discrete
 * input values are updated and 1 is returned.
 * 
 * @param address The 16-bit starting address of the discrete input registers.
 * @param value 0x00 for OFF, and 0x01 for ON.
 * @param count The number of discrete input registers to write.
 * @return int - 1 if successful; -1 if failed.
 */
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
/**
 * @brief Checks if a single discrete input with address is present in the server.
 * 
 * @param address The 16-bit address of the discrete input register.
 * @return true - Discrete input is present in the server.
 * @return false - Discrete input is not found in the server.
 */
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
/**
 * @brief Checks if a range of discrete inputs is present. If any of the discrete inputs
 * is not present in the server, the function returns false.
 * 
 * @param address The 16-bit starting address of the discrete input registers.
 * @param count The number of discrete input registers to check.
 * @return true - All discrete inputs are present in the server.
 * @return false - One or more discrete inputs are not present in the server.
 */
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
/**
 * @brief Reads a single input register in the server. The address is checked for
 * validity.
 * 
 * @param address The 16-bit address of the input register.
 * @return int - The value of the input register; -1 if failed.
 */
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
/**
 * @brief Writes a single input register on the server. The address is checked for
 * validity.
 * 
 * @param address The 16-bit address of the input register.
 * @param value The 16-bit value to write to the input register.
 * @return int - 1 if successful; -1 if failed.
 */
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
/**
 * @brief Writes a contiguous series of input registers on the server. Individual
 * addresses are checked for validity by comparing them with the input register addresses
 * in the input register data array. If any of the addresses are invalid, the function
 * stops writing the values and returns -1. If all the addresses are valid, the input
 * register values are updated and 1 is returned.
 * 
 * @param address The 16-bit starting address of the input registers.
 * @param value The 16-bit value to write to the input registers.
 * @param count The number of input registers to write.
 * @return int - 1 if successful; -1 if failed.
 */
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
/**
 * @brief Checks if a single input register with address is present in the server.
 * 
 * @param address The 16-bit address of the input register to check.
 * @return true - Input register is present in the server.
 * @return false - Input register is not found in the server.
 */
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
/**
 * @brief Checks if a range of input registers is present. If any of the input registers
 * is not present in the server, the function returns false.
 * 
 * @param address The 16-bit starting address of the input registers.
 * @param count The number of input registers to check.
 * @return true - All input registers are present in the server.
 * @return false - One or more input registers are not present in the server.
 */
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
/**
 * @brief Reads a single holding register on the server. The address is checked for
 * validity.
 * 
 * @param address The 16-bit address of the holding register.
 * @return int - The value of the holding register; -1 if failed.
 */
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
/**
 * @brief Writes a single holding register on the server. The address is checked for
 * validity.
 * 
 * @param address The 16-bit address of the holding register.
 * @param value The 16-bit value to write to the holding register.
 * @return int - 1 if successful; -1 if failed.
 */
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
/**
 * @brief Writes a contiguous series of holding registers on the server. Individual
 * addresses are checked for validity by comparing them with the holding register
 * addresses in the holding register data array. If any of the addresses are invalid,
 * the function stops writing the values and returns -1. If all the addresses are valid,
 * the holding register values are updated and 1 is returned.
 * 
 * @param address The 16-bit starting address of the holding registers.
 * @param value The 16-bit value to write to the holding registers.
 * @param count The number of holding registers to write.
 * @return int - 1 if successful; -1 if failed.
 */
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
/**
 * @brief Checks if a single holding register with address is present in the server.
 * 
 * @param address The 16-bit address of the holding register to check.
 * @return true - Holding register is present in the server.
 * @return false - Holding register is not found in the server.
 */
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
/**
 * @brief Checks if a range of holding registers is present. If any of the holding registers
 * is not present in the server, the function returns false.
 * 
 * @param address The 16-bit starting address of the holding registers.
 * @param count The number of holding registers to check.
 * @return true - All holding registers are present in the server.
 * @return false - One or more holding registers are not present in the server.
 */
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
/**
 * @brief Instantiates a new CSE_ModbusRTU_Client object. You must a send a parent
 * CSE_ModbusRTU object and a name for the client.
 * 
 * @param rtu Parent CSE_ModbusRTU object.
 * @param name Name of the client.
 * @return CSE_ModbusRTU_Client:: 
 */
CSE_ModbusRTU_Client:: CSE_ModbusRTU_Client (CSE_ModbusRTU& rtu, String name) {
  this->rtu = &rtu;
  this->name = name;
}

//======================================================================================//
/**
 * @brief Sets the address of the Modbus server you want to communicate with. You
 * must call this function before calling any of the read or write functions.
 * 
 * @param remoteAddress The 8-bit address of the remote server.
 * @return true Operation successful.
 * @return false Operation failed.
 */
bool CSE_ModbusRTU_Client:: setServerAddress (uint8_t remoteAddress) {
  rtu->remoteDeviceAddress = remoteAddress;
  return true;
}

//======================================================================================//
/**
 * @brief Returns the name of the client.
 * 
 * @return String - Name of the client.
 */
String CSE_ModbusRTU_Client:: getName () {
  return name;
}

//======================================================================================//
/**
 * @brief Initializes the client.
 * 
 * @return true - Initialization successful.
 * @return false - Initialization failed.
 */
bool CSE_ModbusRTU_Client:: begin() {
  return true;
}

//======================================================================================//
/**
 * @brief Listen to the serial port and receive the response from the server.
 * 
 * @return int - ADU length if successful; -1 if failed.
 */
int CSE_ModbusRTU_Client:: receive() {
  return rtu->receive (response, receiveTimeout);
}

//======================================================================================//
/**
 * @brief Sends a request to the server.
 * 
 * @return int - ADU length if successful; -1 if failed.
 */
int CSE_ModbusRTU_Client:: send() {
  return rtu->send (request);
}

//======================================================================================//
/**
 * @brief Read a single coil from the server. This function form the request message, sends
 * it to the server and wait for a response. The response ADU is checked for its type
 * and the original function code is returned if the operation is successful. If the
 * response ADU is an exception, the exception code is returned. If the operation fails
 * for other reasons, -1 returned.
 * 
 * @param address The 16-bit address of the coil register.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: readCoil (uint16_t address,  uint8_t count, uint8_t* coilValues) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_READ_COILS); // Function code to read coils
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) count);  // Set the 16-bit quantity of coils to read
  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_READ_COILS) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    uint8_t byteCount = response.getByte (MODBUS_RTU_ADU_DATA_INDEX); // Get the byte count

    // Extract the coil states from the ADU and save it to the coilValues array
    uint16_t coilIndex = 0;

    for (uint8_t i = 0; i < byteCount; i++) {
      uint8_t coilByte = response.getByte (MODBUS_RTU_ADU_DATA_INDEX + 1 + i);  // Read each coil data byte from the ADU

      for (uint8_t j = 0; j < 8; j++) {
        // We only need to read the number of coils requested.
        // More than that will cause an array out of bound error.
        if (coilIndex >= count) {
          break;
        }

        *(coilValues + coilIndex) = (coilByte >> j) & 0x01;  // Save each coil state to the coilValues array
        coilIndex++;
      }
    }

    // Finally return the function code
    return MODBUS_FC_READ_COILS;
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }
  
  return -1;
}

//======================================================================================//
/**
 * @brief Writes a single coil to the server.
 * 
 * @param address The address of the coil register.
 * @param value The value to write to the coil register. Must be 0x0000 or 0xFF00.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: writeCoil (uint16_t address, uint16_t value) {
  if (value > 0x00) {
    value = 0xFF00;
  }

  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_WRITE_SINGLE_COIL); // Function code to write coils
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) value);  // Set the 16-bit value to write. Must be 0x0000 or 0xFF00.
  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    MODBUS_DEBUG_SERIAL.println (F("writeCoil(): Sending request failed!"));
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    MODBUS_DEBUG_SERIAL.println (F("writeCoil(): No response received!"));
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    MODBUS_DEBUG_SERIAL.println (F("writeCoil(): Address mismatch!"));
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_WRITE_SINGLE_COIL) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);
    MODBUS_DEBUG_SERIAL.println (F("writeCoil(): Writing coil successful."));
    return MODBUS_FC_WRITE_SINGLE_COIL;
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    MODBUS_DEBUG_SERIAL.println (F("writeCoil(): Received exception response!"));
    return response.getExceptionCode();
  }

  MODBUS_DEBUG_SERIAL.println (F("writeCoil(): Writing failed!"));
  
  return -1;
}

//======================================================================================//
/**
 * @brief Writes multiple coils to the server. You must first create an uint8_t array of
 * coil values. Each value in the array must be 0x00 or 0x01. The size of the array must
 * be equal to the coil count you want to write.
 * 
 * @param address The starting address of the coil registers.
 * @param count The number of coils to write.
 * @param coilValues A uint8_t array of coil values to write. Each value must be 0x00 or 0x01.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: writeCoil (uint16_t address, uint16_t count, uint8_t* coilValues) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_WRITE_MULTIPLE_COILS); // Function code to write multiple coils
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) count);  // Set the 16-bit quantity of coils to write

  // Now determine the number of bytes we will need to pack the coil values.
  uint8_t byteCount = (uint8_t) (count / 8);

  if (count % 8 > 0) {
    byteCount++;  // If the count is not divisible by 8, then we need to add 1 more byte
  }

  request.add (byteCount);  // Set the number of bytes to follow

  // Now we need to pack the coil values into the request ADU
  for (uint8_t i = 0; i < byteCount; i++) {
    uint8_t coilValue = 0x00; // Start with zero

    for (uint8_t j = 0; j < 8; j++) {
      uint8_t coilIndex = (i * 8) + j;  // Determine the byte position

      if (coilIndex < count) {
        coilValue |= (*(coilValues + coilIndex) << j);  // Set the bit
      }
      // Since we initialize coilValue with zero, we don't need to explicitly clear the bits.
    }

    request.add (coilValue);  // Add each byte to the request ADU
  }

  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_WRITE_MULTIPLE_COILS) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    // Check if the server responded with the same address and count as requested
    if ((response.getWord (MODBUS_RTU_ADU_DATA_INDEX) == address) && (response.getWord (MODBUS_RTU_ADU_DATA_INDEX + 2) == count)) {
      return MODBUS_FC_WRITE_MULTIPLE_COILS;
    }
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }
  
  return -1;
}

//======================================================================================//
/**
 * @brief Reads multiple discrete inputs from the server. You must first create an uint8_t
 * array of size equal to the count you want to read. The function will fill the array with
 * the discrete input values.
 * 
 * @param address The starting address of the discrete input registers.
 * @param count The number of discrete inputs to read.
 * @param inputValues A uint8_t array of size equal to the count you want to read.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: readDiscreteInput (uint16_t address,  uint8_t count, uint8_t* inputValues) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_READ_DISCRETE_INPUTS); // Function code to read discrete inputs
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) count);  // Set the 16-bit quantity of discrete inputs to read
  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_READ_DISCRETE_INPUTS) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    uint8_t byteCount = response.getByte (MODBUS_RTU_ADU_DATA_INDEX); // Get the byte count

    // Now we need to unpack the input values from the response ADU
    uint8_t inputIndex = 0; // Start with zero

    for (uint8_t i = 0; i < byteCount; i++) {
      uint8_t inputByte = response.getByte (MODBUS_RTU_ADU_DATA_INDEX + 1 + i);  // Get each discrete input data byte from the response ADU

      for (uint8_t j = 0; j < 8; j++) {
        // We only need to read the number of discrete inputs requested.
        // More than that will cause an array out of bound error.
        if (inputIndex >= count) {
          break;
        }

        *(inputValues + inputIndex) = (inputByte >> j) & 0x01;  // Save each discrete input state to the inputValues array
        inputIndex++;
      }
    }

    // Finally return the function code
    return MODBUS_FC_READ_DISCRETE_INPUTS;
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }

  return -1;
}

//======================================================================================//
/**
 * @brief Reads multiple holding registers from the server. You must first create an uint16_t
 * array of size equal to the count you want to read. The function will fill the array with
 * the holding register values.
 * 
 * @param address The starting address of the holding registers.
 * @param count The number of holding registers to read.
 * @param inputRegisters A uint16_t array of size equal to the count you want to read.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: readInputRegister (uint16_t address, uint8_t count, uint16_t* inputRegisters) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_READ_INPUT_REGISTERS); // Function code to read input registers
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) count);  // Set the 16-bit quantity of input registers to read
  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_READ_INPUT_REGISTERS) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    // Now we need to unpack the input registers from the response ADU
    for (uint8_t i = 0; i < count; i++) {
      *(inputRegisters + i) = response.getWord ((MODBUS_RTU_ADU_DATA_INDEX + 1) + (i * 2));  // Get each input register data word from the response ADU
    }

    // Finally return the function code
    return MODBUS_FC_READ_INPUT_REGISTERS;
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }

  return -1;
}

//======================================================================================//
/**
 * @brief Reads multiple holding registers from the server. You must first create an uint16_t
 * array of size equal to the count you want to read. The function will fill the array with
 * the holding register values.
 * 
 * @param address The starting address of the holding registers.
 * @param count The number of holding registers to read.
 * @param holdingRegisters A uint16_t array of size equal to the count you want to read.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: readHoldingRegister (uint16_t address, uint8_t count, uint16_t* holdingRegisters) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_READ_HOLDING_REGISTERS); // Function code to read holding registers
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) count);  // Set the 16-bit quantity of holding registers to read
  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }
  
  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_READ_HOLDING_REGISTERS) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    // Now we need to unpack the holding registers from the response ADU
    for (uint8_t i = 0; i < count; i++) {
      *(holdingRegisters + i) = response.getWord ((MODBUS_RTU_ADU_DATA_INDEX + 1) + (i * 2));  // Get each holding register data word from the response ADU
    }

    // Finally return the function code
    return MODBUS_FC_READ_HOLDING_REGISTERS;
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }

  return -1;
}

//======================================================================================//
/**
 * @brief Writes a single holding register to the server.
 * 
 * @param address The address of the holding register.
 * @param value The value to write to the holding register.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: writeHoldingRegister (uint16_t address, uint16_t value) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_WRITE_SINGLE_REGISTER); // Function code to write a single holding register
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) value);  // Set the 16-bit value to write
  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_WRITE_SINGLE_REGISTER) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    // Since the request and response ADUs are the same, we don't need to check anything else.

    return MODBUS_FC_WRITE_SINGLE_REGISTER;
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }

  return -1;
}

//======================================================================================//
/**
 * @brief Writes multiple holding registers to the server. You must first create an uint16_t
 * array of size equal to the count you want to write. The function will write the holding
 * register values from the array.
 * 
 * @param address The starting address of the holding registers.
 * @param count The number of holding registers to write.
 * @param registerValues A uint16_t array of size equal to the count you want to write.
 * @return int - Function code if successful; Exception code if exception; -1 if failed.
 */
int CSE_ModbusRTU_Client:: writeHoldingRegister (uint16_t address, uint16_t count, uint16_t* registerValues) {
  request.resetLength();  // Reset the ADU length to 0
  request.setDeviceAddress (rtu->remoteDeviceAddress);  // The device address is the server address
  request.setFunctionCode (MODBUS_FC_WRITE_MULTIPLE_REGISTERS); // Function code to write multiple holding registers
  request.add ((uint16_t) address);  // Set the 16-bit starting address
  request.add ((uint16_t) count);  // Set the 16-bit quantity of holding registers to write
  request.add ((uint8_t) (count * 2));  // Set the byte count

  // Write the requested number of register values to the request ADU
  for (int i = 0; i < count; i++) {
    request.add (*(registerValues + i));
  }

  request.setCRC(); // Set the CRC

  // If the sending fails, return -1
  if (send() < 0) {
    return -1;
  }

  // If the sending doesn't fail, then try receiving the response.
  // If receiving fails, stop and return -1
  if (receive() < 0) {
    return -1;
  }

  // Check if the response ADU has the same device address as the requested one.
  if (response.getDeviceAddress() != rtu->remoteDeviceAddress) {
    return -1;
  }

  // If sending and receiving doesn't fail, then check the function code in the response ADU.
  // It should be the same as the requested one.
  if (response.getFunctionCode() == MODBUS_FC_WRITE_MULTIPLE_REGISTERS) {
    response.setType (CSE_ModbusRTU_ADU::aduType_t::RESPONSE);

    if (response.getWord (MODBUS_RTU_ADU_DATA_INDEX + 2) == count) {  // Check if the number of registers written is the same as the requested one
      return MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    }
  }
  else if (response.getFunctionCode() > 0x80) { // If the server responded with an exception
    response.setType (CSE_ModbusRTU_ADU::aduType_t::EXCEPTION);
    return response.getExceptionCode();
  }

  return -1;
}

//======================================================================================//

