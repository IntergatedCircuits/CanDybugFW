
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
