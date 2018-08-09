/**
  ******************************************************************************
  * @file    can_if.h
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-31
  * @brief   CanDybug USB interface
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
#ifndef __CAN_IF_H_
#define __CAN_IF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <usbd_cdc.h>
#include <xpd_can.h>

#define CANIF_IN_DATA_SIZE      256
#define CANIF_OUT_DATA_SIZE     128

typedef enum
{
    CANIF_FRAME_SDAT    = CAN_IDTYPE_STD_DATA,
    CANIF_FRAME_SRTR    = CAN_IDTYPE_STD_RTR,
    CANIF_FRAME_EDAT    = CAN_IDTYPE_EXT_DATA,
    CANIF_FRAME_ERTR    = CAN_IDTYPE_EXT_RTR,
    CANIF_ACK_FLAG      = 0x80,
    CANIF_BUS_ERROR     = 0x40,
}CanIfCmdIds;

extern USBD_CDC_IfHandleType *const can_if;

#ifdef __cplusplus
}
#endif

#endif /* __CAN_IF_H_ */
