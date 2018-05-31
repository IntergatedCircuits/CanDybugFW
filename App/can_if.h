
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
