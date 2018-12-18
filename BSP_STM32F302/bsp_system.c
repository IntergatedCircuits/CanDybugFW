
#include <bsp_system.h>
#include <xpd_rcc.h>
#include <xpd_usb.h>

static const RCC_PLL_InitType pllconf = {
    .State = ENABLE,
    .Source = HSE,
    .Multiplier = 72000000 / HSE_VALUE_Hz /* SYSCLK = 72000000 */
};

/* System clocks configuration */
void SystemClock_Config(void)
{
    RCC_eHSE_Config(OSC_ON);
    RCC_ePLL_Config(&pllconf);

    /* System clocks configuration */
    RCC_eHCLK_Config(PLL, CLK_DIV1, 2);

    RCC_vPCLK1_Config(CLK_DIV2);
    RCC_vPCLK2_Config(CLK_DIV1);

    /* USB clock configuration - must be operated from 48 MHz */
    USB_vClockConfig(USB_CLOCKSOURCE_PLL_DIV1p5);
}
