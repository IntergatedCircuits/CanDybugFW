/**
  ******************************************************************************
  * @file    can_if.c
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
#include <can_if.h>
#include <bsp_can.h>
#include <bsp_io.h>
#include <queues.h>
#include <string.h>
#include <leds.h>

static void             Can_Close           (void * itf);
static XPD_ReturnType   Can_Setup           (uint32_t baudrate, FunctionalState trcvPwr, CAN_ModeType testMode);
static XPD_ReturnType   Can_Start           (void);
static boolean_t        Can_HandleRequest   (uint8_t * msg, uint8_t length);
static void             Can_FrameReceived   (void * handle);
static void             Can_FrameSent       (void * handle);
static void             Can_BusError        (void * handle);
static void             Can_PostFrames      (void);

static const CAN_FilterType CanFilters[] = {
    {
        .Pattern.Value = 0,
        .Pattern.Type = CAN_IDTYPE_STD_DATA,
        .Mode = CAN_FILTER_MASK_ANYTYPE,
        .Mask = 0,
        .FIFO = 0,
    },
};
CAN_InitType CanConfig = {
    .Settings.Mode = CAN_MODE_NORMAL,
    .Settings.TXFP = ENABLE,
};

QUEUE_DEF(CanTxQ, CAN_TxMailBox_TypeDef, (CANIF_OUT_DATA_SIZE / 2));

static void CanIf_Open          (void * itf, USBD_CDC_LineCodingType * line);
static void CanIf_EpOutComplete (void * itf, uint8_t * pbuf, uint16_t length);
static void CanIf_EpInComplete  (void * itf, uint8_t * pbuf, uint16_t length);
static void CanIf_AddMessage    (uint8_t * msg, uint8_t length);
static void CanIf_SendInData    (void);
static void CanIf_ReceiveOutData(void);

QUEUE_DEF(CanIf_IN, uint8_t, CANIF_IN_DATA_SIZE);

static struct {
    uint8_t  buffer[CANIF_OUT_DATA_SIZE + 1];
    uint16_t offset;
}CanIf_OUT;

const USBD_CDC_AppType canApp =
{
    .Name           = "CAN bus interface",
    .Open           = CanIf_Open,
    .Received       = CanIf_EpOutComplete,
    .Transmitted    = CanIf_EpInComplete,
    .Close          = Can_Close,
};

USBD_CDC_IfHandleType hcan_if = {
    .App = &canApp,
    .Base.AltCount = 1,
    .Config.Protocol = 0xFF, /* Vendor-specific protocol */
}, *const can_if = &hcan_if;

/**
 * @brief Configure or reset CAN based on reinterpreted CDC commands
 * @param itf: callback sender interface
 * @param line: CAN bus configuration
 */
static void CanIf_Open(void * itf, USBD_CDC_LineCodingType * line)
{
    uint8_t fmi[1];

    GPIO_vInitPin(CAN_SILENT_PIN, CAN_SILENT_CFG);
    GPIO_vInitPin(CAN_TRV_PS_PIN, CAN_TRV_PS_CFG);
    GPIO_vWritePin(CAN_SILENT_PIN, 1);
    GPIO_vWritePin(CAN_TRV_PS_PIN, 0);

    RCC_vClockEnable(Can->CtrlPos);
    RCC_vReset(Can->CtrlPos);

    /* Configure reception filters */
    CAN_eFilterConfig(Can, CanFilters, fmi, 1);

    /* Callbacks subscriptions */
    Can->Callbacks.Receive[0] = Can_FrameReceived;
    Can->Callbacks.Transmit   = Can_FrameSent;
    Can->Callbacks.Error      = Can_BusError;

    if ((XPD_OK == Can_Setup(line->DTERate, line->CharFormat, line->ParityType)) &&
        (XPD_OK == Can_Start()))
    {
        /* empty all buffers */
        CanIf_IN.head = CanIf_IN.tail = 0;
        CanIf_OUT.offset = 0;

        /* Start receiving frames */
        CanIf_ReceiveOutData();

        /* init LEDs */
        Leds_Init();
    }
}

/**
 * @brief Process new USB OUT data by extracting requests
 * @param itf: callback sender interface
 * @param pbuf: Buffer of received data
 * @param length: Number of data received (in bytes)
 */
static void CanIf_EpOutComplete(void * itf, uint8_t * pbuf, uint16_t length)
{
    length += CanIf_OUT.offset;
    pbuf   -= CanIf_OUT.offset;

    /* Cheat to simplify loop exit condition */
    pbuf[length] = 1;

    /* Process all frames from the received data */
    for (; pbuf[0] <= length; length -= pbuf[0], pbuf += pbuf[0])
    {
        if ((pbuf[0] == 0) || (!Can_HandleRequest(pbuf + 1, pbuf[0] - 1)))
        {
            /* Stop receiving data */
            CanIf_OUT.offset = 0;
            return;
        }
    }

    {
        /* Move any request fragment to the beginning */
        memcpy(CanIf_OUT.buffer, pbuf, length);
        CanIf_OUT.offset = length;

        /* Fill empty mailboxes with available queue data */
        Can_PostFrames();

        CanIf_ReceiveOutData();
    }
}

/**
 * @brief Continue receiving new OUT data when there is enough CAN TX FIFO space.
 */
static void CanIf_ReceiveOutData(void)
{
    /* If there is enough FIFO space, and no transmit blocking errors,
     * start receiving new OUT data */
    if ((QUEUE_SPACE(CanTxQ) >= (CANIF_OUT_DATA_SIZE / 4)) &&
        ((CAN_eGetError(Can) & (CAN_ERROR_ERRORPASSIVE | CAN_ERROR_BUSOFF)) == 0))
    {
        (void) USBD_CDC_Receive(can_if,
                &CanIf_OUT.buffer[CanIf_OUT.offset],
                CANIF_OUT_DATA_SIZE - CanIf_OUT.offset);
    }
}

/**
 * @brief Sends available IN data upon completion of the previous.
 * @param itf: callback sender interface
 * @param pbuf: unused
 * @param length: unused
 */
static void CanIf_EpInComplete(void * itf, uint8_t * pbuf, uint16_t length)
{
    CanIf_SendInData();
}

/**
 * @brief This function is called when USB IN transfer completes
 *        or when new IN data is added. It requests new USB IN transfer
 *        if the pipe is free.
 */
static void CanIf_SendInData(void)
{
    if (CanIf_IN.tail != CanIf_IN.head)
    {
        uint32_t newtail;
        uint32_t length;

        /* If the CAN head is ahead, transmit the new data */
        if (CanIf_IN.tail < CanIf_IN.head)
        {
            length  = CanIf_IN.head - CanIf_IN.tail;
            newtail = CanIf_IN.head;
        }
        /* If the USB IN index is ahead, the buffer has wrapped,
         * transmit until the end */
        else
        {
            length  = CanIf_IN.size - CanIf_IN.tail;
            newtail = 0;
        }

        if (USBD_E_OK == USBD_CDC_Transmit(can_if,
                &CanIf_IN.buffer[CanIf_IN.tail + 1], length))
        {
            CanIf_IN.tail = newtail;
        }
    }
}

/**
 * @brief Adds the input data to the CDC IN endpoint transmit queue
 * @param msg: Message buffer
 * @param length: Message length
 */
static void CanIf_AddMessage(uint8_t * msg, uint8_t length)
{
    if (QUEUE_SPACE(CanIf_IN) > length)
    {
        QUEUE_PUT(CanIf_IN, length + 1);
        QUEUE_PUT_ARRAY(CanIf_IN, msg, length);

        CanIf_SendInData();
    }
}

/**
 * @brief This function is called from USB CDC when the device is disconnected.
 * @param itf: callback sender interface
 */
static void Can_Close(void * itf)
{
    CAN_vDeinit(Can);
    GPIO_vDeinitPin(CAN_SILENT_PIN);
    GPIO_vDeinitPin(CAN_TRV_PS_PIN);
    Leds_Deinit();
}

/**
 * @brief Process a single CanIf message.
 * @param msg: Message data
 * @param length: Message length
 * @return TRUE if request is valid, otherwise FALSE
 */
static boolean_t Can_HandleRequest(uint8_t * msg, uint8_t length)
{
    boolean_t retval = FALSE;

    /* Determine request type */
    switch (msg[0])
    {
        case CANIF_FRAME_SDAT:
        case CANIF_FRAME_SRTR:
        case CANIF_FRAME_EDAT:
        case CANIF_FRAME_ERTR:
        {
            /* CAN frame for transmission */
            int16_t  flen = (int16_t)length;
            uint32_t newhead = (CanTxQ.head == CanTxQ.size) ? 0 : CanTxQ.head + 1;
            CAN_TxMailBox_TypeDef * new = &CanTxQ.buffer[newhead];

            /* Standard ID on 2 bytes */
            if ((msg[0] & CAN_IDTYPE_EXT_DATA) == CAN_IDTYPE_STD_DATA)
            {
                /* Set ID register */
                new->TIR.w = msg[0] |
                        (msg[1] << (8 + CAN_TI0R_STID_Pos)) |
                        (msg[2] <<      CAN_TI0R_STID_Pos );

                flen -= 3;
                msg  += 3;
            }
            /* Extended ID on 4 bytes */
            else
            {
                /* Set ID register */
                new->TIR.w = msg[0] |
                        (msg[1] << (24 + CAN_TI0R_EXID_Pos)) |
                        (msg[2] << (16 + CAN_TI0R_EXID_Pos)) |
                        (msg[3] << (8  + CAN_TI0R_EXID_Pos)) |
                        (msg[4] <<       CAN_TI0R_EXID_Pos );

                flen -= 5;
                msg  += 5;
            }

            /* Invalid frame length should cause disconnect */
            if ((flen < 0) || (flen > 8) ||
                (((new->TIR.w & CAN_IDTYPE_STD_RTR) != 0) && (flen != 0)))
            {   break; }

            /* Set DLC, frame data */
            new->TDTR.w = (uint32_t)flen;
            memcpy((void*)&new->TDLR.w, msg, new->TDTR.w);

            CanTxQ.head = newhead;
            retval = TRUE;
            break;
        }

        default:
            break;
    }

    return retval;
}

/**
 * @brief Callback function that handles received CAN frames.
 * @param handle: pointer to invoker CAN handle
 */
static void Can_FrameReceived(void * handle)
{
    /* Keep reading FIFO until empty */
    while (Can->Inst->RFR[0].b.FMP > 0)
    {
        uint8_t *msg;
        uint32_t buf[4];

        /* Read FIFO data */
        uint32_t rir  = Can->Inst->sFIFOMailBox[0].RIR.w;
        uint32_t rdtr = Can->Inst->sFIFOMailBox[0].RDTR.w;

        /* Get the DLC */
        uint8_t len = rdtr & CAN_RDT0R_DLC;

        /* Calculate length and ID fields */
        if ((rir & CAN_IDTYPE_EXT_DATA) == CAN_IDTYPE_STD_DATA)
        {
            msg = ((uint8_t*)buf) + 5;

            /* Due to intel type byte order, ID bytes are reversed for buffer */
            *((uint16_t*)(msg + 1)) = __REV16(rir >> CAN_RI0R_STID_Pos);

            len += 1 + 2;
        }
        else
        {
            msg = ((uint8_t*)buf) + 3;

            /* Due to intel type byte order, ID bytes are reversed for buffer */
            *((uint32_t*)(msg + 1)) = __REV(rir >> CAN_RI0R_EXID_Pos);

            len += 1 + 4;
        }

        msg[0] = rir & CAN_IDTYPE_EXT_RTR;

        /* Copy data to byte buffer */
        buf[2] = Can->Inst->sFIFOMailBox[0].RDLR.w;
        buf[3] = Can->Inst->sFIFOMailBox[0].RDHR.w;

        /* Put the frame in IN pipe if there is enough space */
        CanIf_AddMessage(msg, len);

        /* Release the FIFO */
        CAN_RXFLAG_CLEAR(Can, 0, RFOM);
    }

    /* LED indication */
    Leds_Trigger(GREEN);
}

/**
 * @brief Callback function that handles transmitted CAN frames.
 * @param handle: pointer to invoker CAN handle
 */
static void Can_FrameSent(void * handle)
{
    uint8_t txmb;
    boolean_t ok = FALSE;

    /* check all mailboxes for successful interrupt requests */
    for (txmb = 0; txmb < 3; txmb++)
    {
        if (CAN_TXFLAG_STATUS(Can, txmb, TXOK))
        {
            /* Transmit ACK on USB */
            uint8_t msg[1];
            msg[0] = CANIF_ACK_FLAG |
                (Can->Inst->sTxMailBox[txmb].TIR.w & CAN_IDTYPE_EXT_RTR);

            CanIf_AddMessage(msg, sizeof(msg));

            /* Clear mailbox flags */
            CAN_TXFLAG_CLEAR(Can, txmb, RQCP);

            ok = TRUE;
        }
        else if (CAN_TXFLAG_STATUS(Can, txmb, RQCP))
        {
            /* Clear mailbox flags */
            CAN_TXFLAG_CLEAR(Can, txmb, RQCP);
        }
    }

    if (ok)
    {
        /* Fill empty mailboxes with available queue data */
        Can_PostFrames();

        CanIf_ReceiveOutData();

        /* LED indication */
        Leds_Trigger(GREEN);
    }
}

/**
 * @brief Callback function that handles CAN bus errors.
 * @param handle: pointer to invoker CAN handle
 */
static void Can_BusError(void * handle)
{
    /* Transmit ACK on USB */
    uint8_t msg[2];
    msg[0] = CANIF_BUS_ERROR;
    msg[1] = CAN_eGetError(Can);

    /* Transmit only when a new bus error event was detected */
    if ((msg[1] & CAN_ESR_LEC) != 0)
    {
        CanIf_AddMessage(msg, sizeof(msg));
    }
    /* Clear error interrupt flag */
    CAN_FLAG_CLEAR(Can, ERRI);

    /* LED indication */
    Leds_Trigger(RED);

    /* Stop transmitting at high error rates */
    if ((msg[1] & (CAN_ERROR_ERRORPASSIVE | CAN_ERROR_BUSOFF)) != 0)
    {
        /* Abort all Tx mailboxes */
        uint8_t txmb;
        for (txmb = 0; txmb < 3; txmb++)
        {
            CAN_TXFLAG_CLEAR(Can, txmb, ABRQ);
        }

        /* Reset transmit queue */
        CanTxQ.head = CanTxQ.tail = 0;
    }
}

/**
 * @brief Configure the CAN bus parameters.
 * @param baudrate: bit rate of the bus
 * @param trcvPwr: whether the CAN transceiver should be USB powered
 * @return OK if the baudrate is valid, otherwise ERROR
 */
static XPD_ReturnType Can_Setup(uint32_t baudrate, FunctionalState trcvPwr, CAN_ModeType testMode)
{
    XPD_ReturnType result = CAN_eBitrateConfig(baudrate, &CanConfig.Timing);

    if (XPD_OK == result)
    {
        CanConfig.Settings.Mode = testMode;

        /* Set transceiver power */
        GPIO_vWritePin(CAN_TRV_PS_PIN, trcvPwr);

        /* Silent mode */
        GPIO_vWritePin(CAN_SILENT_PIN, 0);
    }
    return result;
}

/**
 * @brief Starts the CAN bus operation.
 * @return The result of the bus synchronization
 */
static XPD_ReturnType Can_Start(void)
{
    /* Initialize CAN peripheral */
    XPD_ReturnType result = CAN_eInit(Can, &CanConfig);

    /* Enable interrupts */
    Can->Inst->IER.w = CAN_IER_FMPIE0 | CAN_IER_TMEIE | CAN_IER_ERRIE
            | CAN_IER_LECIE;

    /* Reset transmit queue */
    CanTxQ.head = CanTxQ.tail = 0;

    return result;
}

/**
 * @brief This function fills the CAN peripheral mailboxes with queue data.
 */
static void Can_PostFrames(void)
{
    while(!QUEUE_EMPTY(CanTxQ))
    {
        uint32_t tsr = Can->Inst->TSR.w;

        /* Check if at least one mailbox is empty */
        if ((tsr & CAN_TSR_TME) != 0)
        {
            uint8_t mailbox = (tsr & CAN_TSR_CODE) >> CAN_TSR_CODE_Pos;

            /* Copy mailbox data */
            Can->Inst->sTxMailBox[mailbox] = QUEUE_GET(CanTxQ);

            /* request transmission */
            CAN_REG_BIT(Can,sTxMailBox[mailbox].TIR,TXRQ) = 1;
        }
        else break;
    }
}
