/**
  ******************************************************************************
  * @file    bsp_system.c
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug BSP for system clocking
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
#include <bsp_system.h>
#include <xpd_rcc.h>
#include <xpd_usb.h>

#ifndef HSE_VALUE_Hz
#include <xpd_crs.h>
static const CRS_InitType crsSetup = {
    .Source     = CRS_SYNC_SOURCE_USB,
    .ErrorLimit = CRS_ERRORLIMIT_DEFAULT,
};
#else
static const RCC_PLL_InitType pllconf = {
    .State = ENABLE,
    .Source = HSE,
    .Predivider = 1,
    .Multiplier = 48000000 / HSE_VALUE_Hz,
};
#endif

/** @brief System clocks configuration */
void SystemClock_Config(void)
{
#ifndef HSE_VALUE_Hz
    RCC_eHSI48_Enable();
    CRS_vInit(&crsSetup);

    /* System clocks configuration */
    RCC_eHCLK_Config(HSI48, CLK_DIV1, 1);
#else
    RCC_eHSE_Config(OSC_ON);
    RCC_ePLL_Config(&pllconf);

    /* System clocks configuration */
    RCC_eHCLK_Config(PLL, CLK_DIV1, 1);
#endif
    RCC_vPCLK1_Config(CLK_DIV1);

    /* USB clock configuration - must be operated from 48 MHz */
#ifndef HSE_VALUE_Hz
    USB_vClockConfig(USB_CLOCKSOURCE_HSI48);
#else
    USB_vClockConfig(USB_CLOCKSOURCE_PLL);
#endif
}
