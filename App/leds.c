/**
  ******************************************************************************
  * @file    leds.c
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-09-15
  * @brief   CanDybug LEDs
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
#include <leds.h>
#include <bsp_tim.h>

static TIM_InitType time = {
    .Period = 10000,
    .Mode = TIM_COUNTER_CENTERALIGNED3,
    .Preload = ENABLE,
    .OnePulse = ENABLE,
};
static const TIM_ChannelType ledCh[] = { LED_RED_CH, LED_GREEN_CH };
static LedsType lastLed;

void Leds_Init(void)
{
    lastLed = RED + GREEN + 1;

    /* Prescale to 100 kHz counts */
    time.Prescaler = TIM_ulClockFreq_Hz(LedTimer) / 100000;

    TIM_vInit(LedTimer, &time,
            LedChConfig, sizeof(LedChConfig)/sizeof(LedChConfig[0]));

    /* Give a startup flash */
    *TIM_pulChannel(LedTimer, ledCh[RED]) = time.Period - 2;
    *TIM_pulChannel(LedTimer, ledCh[GREEN]) = time.Period - 2;
    TIM_vGenerate(LedTimer, TIM_EVENT_UPDATE);
    TIM_vStart(LedTimer);
}

void Leds_Deinit(void)
{
    TIM_vDeinit(LedTimer);
}

void Leds_Trigger(LedsType led)
{
    /* Only one channel lights at a time */
    if (led != lastLed)
    {
        *TIM_pulChannel(LedTimer, ledCh[1 - led]) = 0;
        *TIM_pulChannel(LedTimer, ledCh[led]) = time.Period / 4;
    }

    if ((led != lastLed) || !TIM_bCounterIsRunning(LedTimer))
    {
        /* Load new channel values and start pulse */
        TIM_vGenerate(LedTimer, TIM_EVENT_UPDATE);
        TIM_vStart(LedTimer);
    }

    lastLed = led;
}
