
#include <bsp_can.h>
#include <bsp_io.h>
#include <xpd_nvic.h>

void USB_HP_CAN_TX_IRQHandler(void);
void USB_LP_CAN_RX0_IRQHandler(void);
void CAN_SCE_IRQHandler(void);

static void BSP_CAN_Init(void * handle)
{
    /* GPIO settings */
    GPIO_vInitPin(CAN_TX_PIN, CAN_TX_CFG);
    GPIO_vInitPin(CAN_RX_PIN, CAN_RX_CFG);

    NVIC_SetPriorityConfig(USB_HP_CAN_TX_IRQn, 0, 0);
    NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
    NVIC_SetPriorityConfig(USB_LP_CAN_RX0_IRQn, 0, 0);
    NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
    NVIC_SetPriorityConfig(CAN_SCE_IRQn, 0, 0);
    NVIC_EnableIRQ(CAN_SCE_IRQn);
}

/* CAN dependencies deinitialization */
static void BSP_CAN_Deinit(void * handle)
{
    GPIO_vDeinitPin(CAN_TX_PIN);
    GPIO_vDeinitPin(CAN_RX_PIN);
    NVIC_DisableIRQ(USB_HP_CAN_TX_IRQn);
    NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn);
    NVIC_DisableIRQ(CAN_SCE_IRQn);
}

CAN_HandleType hcan, *const Can = &hcan;

void BSP_CAN_Bind(void)
{
    CAN_INST2HANDLE(Can, CAN);
    Can->Callbacks.DepInit = BSP_CAN_Init;
    Can->Callbacks.DepDeinit = BSP_CAN_Deinit;
}

/* CAN interrupt handling */
void USB_HP_CAN_TX_IRQHandler(void)
{
    /* transmit complete callback */
    XPD_SAFE_CALLBACK(Can->Callbacks.Transmit, &Can);
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
    /* receive complete callback */
    XPD_SAFE_CALLBACK(Can->Callbacks.Receive[0], &Can);
}

void CAN_SCE_IRQHandler(void)
{
    XPD_SAFE_CALLBACK(Can->Callbacks.Error, &Can);
}
