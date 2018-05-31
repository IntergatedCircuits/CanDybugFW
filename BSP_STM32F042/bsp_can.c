/**
  ******************************************************************************
  * @file    bsp_can.c
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
#include <bsp_can.h>
#include <bsp_io.h>
#include <xpd_nvic.h>

void CEC_CAN_IRQHandler(void);

/* CAN dependencies initialization */
static void BSP_CAN_Init(void * handle)
{
    /* GPIO settings */
    GPIO_vInitPin(CAN_TX_PIN, CAN_TX_CFG);
    GPIO_vInitPin(CAN_RX_PIN, CAN_RX_CFG);

    NVIC_SetPriorityConfig(CEC_CAN_IRQn, 0, 0);
    NVIC_EnableIRQ(CEC_CAN_IRQn);
}

/* CAN dependencies deinitialization */
static void BSP_CAN_Deinit(void * handle)
{
    GPIO_vDeinitPin(CAN_TX_PIN);
    GPIO_vDeinitPin(CAN_RX_PIN);
    NVIC_DisableIRQ(CEC_CAN_IRQn);
}

CAN_HandleType hcan, *const Can = &hcan;

void BSP_CAN_Bind(void)
{
    CAN_INST2HANDLE(Can, CAN);
    Can->Callbacks.DepInit = BSP_CAN_Init;
    Can->Callbacks.DepDeinit = BSP_CAN_Deinit;
}

/* CAN interrupt signaling */
void CEC_CAN_IRQHandler(void)
{
    if ((Can->Inst->TSR.w & (CAN_TSR_RQCP0 | CAN_TSR_RQCP1 | CAN_TSR_RQCP2)) != 0)
    {
        /* transmit complete callback */
        XPD_SAFE_CALLBACK(Can->Callbacks.Transmit, Can);
    }
    if (Can->Inst->RFR[0].w != 0)
    {
        /* receive complete callback */
        XPD_SAFE_CALLBACK(Can->Callbacks.Receive[0], Can);
    }
    if ((Can->Inst->ESR.w & (CAN_ESR_LEC | CAN_ESR_BOFF | CAN_ESR_EPVF | CAN_ESR_EWGF)) != 0)
    {
        XPD_SAFE_CALLBACK(Can->Callbacks.Error, Can);
    }
}
