/**
  ******************************************************************************
  * @file    bsp_io.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug BSP for I/O pins
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
#ifndef __BSP_IO_H_
#define __BSP_IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_gpio.h>

#define CAN_TX_PIN          GPIOB, 9
#define CAN_TX_CFG          (&BSP_IOCfg[1])
#define CAN_RX_PIN          GPIOB, 8
#define CAN_RX_CFG          (&BSP_IOCfg[1])

#define CAN_SILENT_PIN      GPIOC, 13
#define CAN_SILENT_CFG      (&BSP_IOCfg[0])
#define CAN_TRV_PS_PIN      GPIOA, 10
#define CAN_TRV_PS_CFG      (&BSP_IOCfg[0])

#define USB_DP_PIN          GPIOA, 12
#define USB_DP_CFG          (&BSP_IOCfg[2])
#define USB_DM_PIN          GPIOA, 11
#define USB_DM_CFG          (&BSP_IOCfg[2])

extern const GPIO_InitType BSP_IOCfg[];

#ifdef __cplusplus
}
#endif

#endif /* __BSP_IO_H_ */
