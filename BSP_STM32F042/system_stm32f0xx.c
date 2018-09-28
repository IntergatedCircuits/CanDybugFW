/**
  ******************************************************************************
  * @file    system_stm32f0xx.c
  * @author  Benedek Kupper
  * @version 0.2
  * @date    2018-01-28
  * @brief   STM32 eXtensible Peripheral Drivers template
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
#include <xpd_flash.h>
#include <xpd_rcc.h>
#include <xpd_syscfg.h>
#include <xpd_utils.h>

/** @brief Global variable used to store the actual system clock frequency [Hz] */
uint32_t SystemCoreClock;

/** @brief Interrupt vector table in flash */
extern const uint32_t g_pfnVectors[];

uint32_t __attribute__((section (".isr_vector_sram"))) g_pfnVectorsCpy[48];

/**
 * @brief Copies the vector table to the beginning of SRAM
 *        and remaps 0 address to SRAM in SYSCFG.
 */
static void VectorTableRemap(void)
{
    int i = 48; /* CM0 has fixed 16 core and 32 vendor-specific interrupt vectors */
    int *src = (int*)g_pfnVectors, *desc = (int*)g_pfnVectorsCpy;
    while (i--)
        *desc++ = *src++;

    /* Set Memory Mode to SRAM (0x20000000 is remapped to 0x00000000) */
    SYSTEM_MEMORY_REMAP(SRAM);
}

/**
 * @brief  Setup the microcontroller system.
 *         Initialize the default HSI clock source,
 *         vector table location
 *         and the PLL configuration is reset.
 */
void SystemInit(void)
{
    /* Reset all peripherals */
    XPD_vDeinit();

    /* Reset the RCC clock configuration to the default reset state
     * Sets SystemCoreClock value */
    RCC_vDeinit();

    /* initialize XPD services */
    XPD_vInit();

    /* Due to application offset in flash,
     * interrupt vectors have to be mapped to SRAM */
    VectorTableRemap();

    /* TODO Configure system memory options */
    FLASH_vPrefetchBuffer(ENABLE);
}
