/**
  ******************************************************************************
  * @file    bsp_tim.c
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
#include <bsp_tim.h>
#include <bsp_io.h>

static TIM_HandleType ledtimer;
TIM_HandleType *const LedTimer = &ledtimer;

const TIM_ChannelInitType LedChConfig[2] = {
    {.OC = {
        .Number = LED_RED_CH,
        .Mode = TIM_OUTPUT_PWM1,
        .Polarity = ACTIVE_HIGH,
        .InvChannel = ENABLE,
        .InvPolarity = ACTIVE_LOW,
        .Value = 0,
    }},
    {.OC = {
        .Number = LED_GREEN_CH,
        .Mode = TIM_OUTPUT_PWM1,
        .Polarity = ACTIVE_HIGH,
        .InvChannel = ENABLE,
        .InvPolarity = ACTIVE_LOW,
        .Value = 0,
    }},
};

static void BSP_LedTimer_Init(void * handle)
{
    GPIO_vInitPin(LED_RED_PIN, LED_RED_CFG);
    GPIO_vInitPin(LED_GREEN_PIN, LED_GREEN_CFG);
}

static void BSP_LedTimer_Deinit(void * handle)
{
    GPIO_vDeinitPin(LED_RED_PIN);
    GPIO_vDeinitPin(LED_GREEN_PIN);
}

void BSP_LedTimer_Bind(void)
{
    TIM_INST2HANDLE(LedTimer, TIM1);
    LedTimer->Callbacks.DepInit = BSP_LedTimer_Init;
    LedTimer->Callbacks.DepDeinit = BSP_LedTimer_Deinit;
}
