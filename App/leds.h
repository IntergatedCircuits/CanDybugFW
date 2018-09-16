/**
  ******************************************************************************
  * @file    leds.h
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
#ifndef LEDS_H_
#define LEDS_H_

typedef enum
{
    RED,
    GREEN,
}LedsType;

void Leds_Init(void);
void Leds_Deinit(void);
void Leds_Trigger(LedsType led);

#endif /* LEDS_H_ */
