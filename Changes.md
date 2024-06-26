
# Changes

Change log for `CSE_ModbusRTU` library. Latest entries are at the top.

#
### **+05:30 12:44:25 AM 21-05-2024, Tuesday**

  * Added include guard.

#
### **+05:30 12:25:48 AM 16-03-2024, Saturday**

  * Implemented dynamic debug message printing functionality. The user can now enable/disable the debug messages on the fly.

#
### **+05:30 11:37:40 PM 14-03-2024, Thursday**

  * Fixed array out of bounds error when sending back register data.
  * Added test sketches.

#
### **+05:30 10:14:04 AM 26-02-2024, Monday**

  * Added global switch macro for enabling/disabling debug messages.

#
### **+05:30 07:17:59 PM 21-01-2024, Sunday**

  * Added AVR examples.
  * Fixed missing byte count in responses. [Issue #4](https://github.com/CIRCUITSTATE/CSE_ModbusRTU/issues/4).
  * Added AVR standard vector library support. [Issue #2](https://github.com/CIRCUITSTATE/CSE_ModbusRTU/issues/2)

#
### **+05:30 03:04:48 PM 30-10-2023, Monday**

  * Fixed `CSE_ModbusRTU::receive()` not asserting RE pin issue.
  * Added new `enableReceive()` and `disableReceive()` functions to `CSE_ModbusRTU` class.
  * Receiving is disabled with `disableReceive()` in `CSE_ModbusRTU_Client:: receive()` after a response is received or timeout occurs.
  * None of the changes are breaking.
  * New version **`0.0.7`**.

#
### **+05:30 03:18:19 PM 21-10-2023, Saturday**

  * Updated documentation.
  * Updated library information.
  * New version **`0.0.6`**.

#
### **+05:30 09:18:37 AM 19-08-2023, Saturday**

  * Fixed non-void function without return value.
  * New version **`0.0.5`**.

#
### **+05:30 10:39:44 PM 15-08-2023, Tuesday**

  * Updated library meta.
  * New version **`0.0.4`**.

#
### **+05:30 11:40:31 PM 13-08-2023, Sunday**

  * Added more debug messages.
  * Added `isCRCSet` to `calculateCRC()` to tell if the CRC is set or not.
  * Added `print()' to `CSE_ModbusRTU_ADU` to print the ADU as a hex string.
  * Added receive timeout to `CSE_ModbusRTU:: receive`.
  * Fixed issues with `CSE_ModbusRTU:: send` by adding begin and end transactions.
  * Added `CSE_ModbusRTU_Client` class.
  * Examples updated and working.
  * New version **`0.0.3`**.

#
### **+05:30 08:41:17 PM 02-08-2023, Wednesday**

  * Added more documentation.
  * New version **`0.0.2`**.

