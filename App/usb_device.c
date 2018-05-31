/**
  ******************************************************************************
  * @file    usb_device.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug USB Device definition and configuration
  *
  * Copyright (c) 2018 Benedek Kupper
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include <xpd_usb.h>
#include <usbd.h>
#include <can_if.h>

#include <usbd_dfu.h>
/* DFU interface is initialized by bootloader */
USBD_DFU_IfHandleType __attribute__((section (".dfuSharedSection"))) hdfu_if;
USBD_DFU_IfHandleType *const dfu_if = &hdfu_if;


/** @brief USB device configuration */
const USBD_DescriptionType hdev_cfg = {
    .Vendor = {
        .Name           = "IntergatedCircuits",
        .ID             = 0xffff, /* TODO placeholder */
    },
    .Product = {
        .Name           = "CanDybug",
        .ID             = 0xffff, /* TODO placeholder */
        .Version        = {{ 1,0xC }},
    },
    .SerialNumber       = (USBD_SerialNumberType*)DEVICE_ID_REG,
    .Config = {
        .Name           = "CanDybug",
        .MaxCurrent_mA  = 100,
        .RemoteWakeup   = 0,
        .SelfPowered    = 0,
    },
}, *const dev_cfg = &hdev_cfg;

/** @brief USB device handle */
USBD_HandleType hUsbDevice, *const UsbDevice = &hUsbDevice;

/**
 * @brief This function handles the setup of the USB device:
 *         - Assigns endpoints to USB interfaces
 *         - Mounts the interfaces on the device
 *         - Sets up the USB device
 *         - Determines the USB port type
 *         - Establishes logical connection with the host
 */
void UsbDevice_Init(void)
{
    USB_ChargerType usbPort;

    /* Initialize the device */
    USBD_Init(UsbDevice, dev_cfg);

    /* Set the available current limit based on the USB connection type */
    usbPort = USB_eChargerDetect(UsbDevice);

    switch (usbPort)
    {
        /* No host is present, don't setup the interfaces */
        case USB_BCD_DEDICATED_CHARGING_PORT:
        case USB_BCD_PS2_PROPRIETARY_PORT:
            USBD_Deinit(UsbDevice);
            break;

        default:
        {
            /* All fields of Config have to be properly set up */
            can_if->Config.InEpNum  = 0x81;
            can_if->Config.OutEpNum = 0x01;
            can_if->Config.NotEpNum = 0x8F;

            USBD_DFU_AppInit(dfu_if, 100); /* Detach can be carried out within 100 ms */

            /* Mount the interfaces to the device */
            USBD_DFU_MountInterface(dfu_if, UsbDevice);
            USBD_CDC_MountInterface(can_if, UsbDevice);

            /* After charger detection the device connection can be made */
            USBD_Connect(UsbDevice);
            break;
        }
    }
}

/**
 * @brief Shuts down the USB peripheral.
 */
void UsbDevice_Deinit(void)
{
    USBD_Deinit(UsbDevice);
}
