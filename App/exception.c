/**
  ******************************************************************************
  * @file    exception.c
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-09-16
  * @brief   Exception handling
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
#include <xpd_nvic.h>
#include <usbd_dfu.h>
extern USBD_DFU_IfHandleType *const dfu_if;

void HardFault_Handler(void);

void HardFault_Handler(void)
{
    /* Reset to bootloader */
    dfu_if->Tag[0] = DFU_MODE_TAG;
    dfu_if->Tag[1] = ~DFU_MODE_TAG;
    NVIC_SystemReset();
}
