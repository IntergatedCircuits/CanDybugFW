/**
  ******************************************************************************
  * @file    bsp_can.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug BSP for CAN bus peripheral
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
#ifndef __BSP_CAN_H_
#define __BSP_CAN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_can.h>

extern CAN_HandleType *const Can;

void BSP_CAN_Bind(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CAN_H_ */
