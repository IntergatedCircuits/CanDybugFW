/**
  ******************************************************************************
  * @file    bsp_tim.h
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-09-15
  * @brief   CanDybug BSP for LED timer
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
#ifndef __BSP_TIM_H_
#define __BSP_TIM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_tim.h>

#define LED_RED_CH      TIM_CH2
#define LED_GREEN_CH    TIM_CH1

extern TIM_HandleType *const LedTimer;
extern const TIM_ChannelInitType LedChConfig[2];

void BSP_LedTimer_Bind(void);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_TIM_H_ */
