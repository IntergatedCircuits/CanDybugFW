#ifndef __BSP_USB_H_
#define __BSP_USB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_usb.h>

extern USB_HandleType *const UsbDevice;

void BSP_USB_Bind(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_USB_H_ */
