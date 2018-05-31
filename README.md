# CanDybugFW

This repository contains the embedded firmware of the CanDybug,
a CAN bus traffic analizer which uses a custom protocol over a USB serial port emulation.

## USB Protocol

The device appears as a standard virtual COM port (CDC-ACM with vendor-specific protocol) when connected to a host machine. 
The setup of the interface is done through control requests, 
while the bus activity is communicated through the dedicated data endpoints.

### I. CAN bus setup

Setting up the CAN bus parameters is analogical to the serial port setup. The same request format is used
(get = 0x21, set = 0x20), but the interpretation of certain values is changed for this application.

Data = {dwBaudrate (4), bTrcvPwr */Stop bit/* (1), bTestMode */Parity/* (1), bDataBits (1)}

1. **dwBaudrate** baudrate of the CAN bus in bit/s
2. **bTrcvPwr** when set, connects USB 5V to the on-board CAN transceiver
3. **bTestMode** 0 for normal operation, 1 for loopback 
4. **bDataBits** read-only, returns 64

### II. Data pipe packets

The data channel has a specific message format. The first byte is always the total length of the message (including the length byte). The second byte is the message type, and the rest is type dependent, as shown in the table below:

| Length | Code | Message type             | Payload |
|:------:|:----:|:-------------------------|:--------|
| 4..12  | 0x00 | CAN standard data frame  | {2 byte ID; 0..8 byte data} |
| 4		 | 0x02 | CAN standard RTR frame   | {2 byte ID} |
| 6..14  | 0x04 | CAN extended data frame  | {4 byte ID; 0..8 byte data} |
| 6      | 0x06 | CAN extended RTR frame   | {4 byte ID} |
|--------|------|--------------------------|---------------------|
| 3      | 0x40 | CAN bus error 		   | {1 byte error info} |
| 2      | 0x8x | CAN frame sent by device | {} |

The last two types are only sent by the device, the others are both sent by the host to initiate a CAN frame transmission on the bus and by the device when it read a CAN frame from the bus.

## Hardware design

The PCB with its respective BOM is available on [OSHPark](https://oshpark.com/shared_projects/jVh7PNHF).

## External dependencies

This firmware relies on the following external sources:
1. The **CMSIS** and **STM32F{x}_XPD** libraries are imported from [STM32_XPD](https://github.com/IntergatedCircuits/STM32_XPD)
2. The USB device library (including CDC and DFU classes) are imported from [USBDevice](https://github.com/IntergatedCircuits/USBDevice)
3. The [DFU bootloader](https://github.com/IntergatedCircuits/DfuBootloader) is used with the following parameters:
`FLASH_APP_ADDRESS=0x08002000, FLASH_APP_SIZE=24568, BSP=STM32F0x2, FLASH_BYTE_PROGRAM_TIME_us=30, FLASH_TOTAL_ERASE_TIME_ms=480, USBD_VID=0xFFFF, USBD_PID=0xFFFF, VDD_VALUE_mV=3300, core=cm0`
