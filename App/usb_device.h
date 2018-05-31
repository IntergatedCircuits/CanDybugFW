/**
  ******************************************************************************
  * @file    usb_device.h
  * @author  Benedek Kupper
  * @version 1.0
  * @date    2018-05-31
  * @brief   CanDybug USB Device header
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
#ifndef __USB_DEVICE_H_
#define __USB_DEVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <usbd_types.h>

extern USBD_HandleType *const UsbDevice;
void UsbDevice_Init(void);
void UsbDevice_Deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE_H_ */
