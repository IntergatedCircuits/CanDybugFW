/**
  ******************************************************************************
  * @file    main.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug main function
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
#include <xpd_pwr.h>
#include <xpd_systick.h>

#include <bsp_can.h>
#include <bsp_system.h>
#include <bsp_usb.h>

#include <usb_device.h>

/* Initialize the system then enter Sleep
 * and let interrupts handle everything */
int main(void)
{
    /* Initialize BSP variables */
    BSP_CAN_Bind();
    BSP_USB_Bind();
    /* TODO Bind LEDs peripherals */

    /* Configure system clocks */
    SystemClock_Config();

    /* Enable USB device */
    UsbDevice_Init();

    while (1)
    {
        /* Sleep here, DMA and interrupts handle everything */
        __WFI();
    }
}
