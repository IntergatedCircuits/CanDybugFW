/**
  ******************************************************************************
  * @file    xpd_config.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug STM32F0_XPD configuration header
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
#ifndef XPD_CONFIG_H_
#define XPD_CONFIG_H_

/* TODO step 1: specify device header */
#include <stm32f042x6.h>

/* TODO step 2: specify power supplies */
#define VDD_VALUE_mV                   3300 /* Value of VDD in mV */

/* TODO step 3: specify oscillator parameters */
#define HSE_VALUE_Hz 120000000

#endif /* XPD_CONFIG_H_ */
