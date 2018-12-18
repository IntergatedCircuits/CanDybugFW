/**
  ******************************************************************************
  * @file    bsp_usb.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug BSP for USB communication
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
#include <bsp_io.h>
#include <bsp_usb.h>
#include <xpd_nvic.h>

void USB_IRQHandler(void);

static const EXTI_InitType usbWakeup = {
        .Edge       = EDGE_RISING,
        .Reaction   = REACTION_IT,
};

/* USB dependencies initialization */
static void BSP_USB_Init(void * handle)
{
    /* GPIO settings */
    GPIO_vInitPin(USB_DM_PIN, USB_DM_CFG);
    GPIO_vInitPin(USB_DP_PIN, USB_DP_CFG);

    /* Only necessary for low pin count devices */
    GPIO_PIN_REMAP(PA11_PA12);

    /* Configure USB wakeup */
    EXTI_vInit(USB_WAKEUP_EXTI_LINE, &usbWakeup);

    /* Enable USB FS Interrupt */
    NVIC_SetPriorityConfig(USB_IRQn, 0, 0);
    NVIC_EnableIRQ(USB_IRQn);
}

/* USB dependencies deinitialization */
static void BSP_USB_Deinit(void * handle)
{
    GPIO_vDeinitPin(USB_DM_PIN);
    GPIO_vDeinitPin(USB_DP_PIN);
    NVIC_DisableIRQ(USB_IRQn);
}

void BSP_USB_Bind(void)
{
    USB_INST2HANDLE(UsbDevice, USB);
    UsbDevice->Callbacks.DepInit = BSP_USB_Init;
    UsbDevice->Callbacks.DepDeinit = BSP_USB_Deinit;
}

/* Common interrupt handler for USB core and WKUP line */
void USB_IRQHandler(void)
{
    EXTI_vClearFlag(USB_WAKEUP_EXTI_LINE);

    /* Handle USB interrupts */
    USB_vIRQHandler(UsbDevice);
}
