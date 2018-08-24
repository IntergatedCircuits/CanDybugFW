
#include <bsp_io.h>
#include <bsp_usb.h>
#include <xpd_nvic.h>

void USB_HP_IRQHandler(void);
void USB_LP_IRQHandler(void);
void USBWakeUp_RMP_IRQHandler(void);

/* This STM32 line doesn't have built-in DP pull-up,
 * therefore an external 1.5kOhm resistor must be placed between USB DP
 * and a GPIO pin.
 */
static void usbAttachDetach(FunctionalState state)
{
    GPIO_vWritePin(USB_CONNECT_PIN, state);
}

/* USB dependencies initialization */
static void BSP_USB_Init(void * handle)
{
    /* GPIO settings */
    GPIO_vInitPin(USB_DM_PIN, USB_DM_CFG);
    GPIO_vInitPin(USB_DP_PIN, USB_DP_CFG);
    GPIO_vInitPin(USB_CONNECT_PIN, USB_CONNECT_CFG);

    /* USB clock configuration - must be operated from 48 MHz */
    USB_vClockConfig(USB_CLOCKSOURCE_PLL_DIV1p5);

    /* Enable USB FS Interrupt */
    USB_IT_REMAP(ENABLE);

    NVIC_SetPriorityConfig(USB_HP_IRQn, 0, 0);
    NVIC_EnableIRQ(USB_HP_IRQn);
    NVIC_SetPriorityConfig(USB_LP_IRQn, 0, 0);
    NVIC_EnableIRQ(USB_LP_IRQn);

    //EXTI_vInit(USB_WAKEUP_EXTI_LINE, &wakeup);

    NVIC_SetPriorityConfig(USBWakeUp_RMP_IRQn, 0, 0);
    NVIC_EnableIRQ(USBWakeUp_RMP_IRQn);
}

/* USB dependencies deinitialization */
static void BSP_USB_Deinit(void * handle)
{
    GPIO_vDeinitPin(USB_DM_PIN);
    GPIO_vDeinitPin(USB_DP_PIN);
    NVIC_DisableIRQ(USB_HP_IRQn);
    NVIC_DisableIRQ(USB_LP_IRQn);
    NVIC_DisableIRQ(USBWakeUp_RMP_IRQn);
}

void BSP_USB_Bind(void)
{
    USB_INST2HANDLE(UsbDevice, USB);
    UsbDevice->Callbacks.DepInit = BSP_USB_Init;
    UsbDevice->Callbacks.DepDeinit = BSP_USB_Deinit;
    UsbDevice->Callbacks.ConnectCtrl = usbAttachDetach;
}

/* Common interrupt handler for USB core and WKUP line */
void USB_HP_IRQHandler(void)
{
    /* Handle USB interrupts */
    USB_vIRQHandler(UsbDevice);
}
void USB_LP_IRQHandler(void)
{
    /* Handle USB interrupts */
    USB_vIRQHandler(UsbDevice);
}
void USBWakeUp_RMP_IRQHandler(void)
{
    EXTI_vClearFlag(USB_WAKEUP_EXTI_LINE);
    USB_vIRQHandler(UsbDevice);
}
