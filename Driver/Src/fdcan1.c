/**
 * @file fdcan1.c
 * @brief FDCAN1 CAN 2.0 driver for STM32G0B1 @ 64 MHz.
 *
 * Pins: PB8 RX / PB9 TX (AF3)
 * Bitrate: 500 kbps nominal phase
 *
 * This is a minimal CAN 2.0 implementation (not CAN FD).
 * Both Rx FIFO 0 and message reception trigger an interrupt.
 */
#include "fdcan1.h"
#include <stddef.h>

/* GPIO helper */
static void gpio_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    MODIFY_REG(port->MODER,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    CLEAR_BIT(port->OTYPER, (1UL << pin));
    MODIFY_REG(port->OSPEEDR,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    CLEAR_BIT(port->PUPDR, (0x3UL << (pin * 2U)));
    if (pin < 8U) {
        MODIFY_REG(port->AFR[0],
                   (0xFUL << (pin * 4U)),
                   (af    << (pin * 4U)));
    } else {
        uint32_t p = pin - 8U;
        MODIFY_REG(port->AFR[1],
                   (0xFUL << (p * 4U)),
                   (af    << (p * 4U)));
    }
}

static FDCAN_Callback_t fdcan1_callback = NULL;

void FDCAN1_Init(void)
{
    /* Enable GPIOB clock */
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);

    /* PB8 RX, PB9 TX — AF3 */
    gpio_af(GPIOB, 8U, 3U);
    gpio_af(GPIOB, 9U, 3U);

    /* Enable FDCAN1 clock on APB1 */
    SET_BIT(RCC->APBENR1, RCC_APBENR1_FDCAN1EN);

    /* Enter configuration mode (CCCR.INIT = 1, CCE = 1) */
    SET_BIT(FDCAN1->CCCR, 0x1UL);        /* INIT */
    while (!(READ_BIT(FDCAN1->CCCR, 0x1UL))) {}

    SET_BIT(FDCAN1->CCCR, 0x2UL);        /* CCE (config enable) */

    /* Configure nominal phase bitrate for 500 kbps
     * FDCAN clock = 64 MHz (APB1)
     * Prescaler = 8 → 8 MHz timer clock
     * tq = 1/8MHz = 125 ns
     * 500 kbps → bit time = 2000 ns = 16 tq
     * NBRP = 7 (prescaler - 1)
     * NTSEG1 = 11 (12 tq)
     * NTSEG2 = 3 (4 tq)
     * NSJW = 3 (4 tq)
     */
    WRITE_REG(FDCAN1->NBTP,
              (7U << 0)   |   /* NBRP */
              (11U << 8)  |   /* NTSEG1 */
              (3U << 16)  |   /* NTSEG2 */
              (3U << 24));    /* NSJW */

    /* Configure Rx FIFO 0: standard frame acceptance (11-bit ID)
     * FLSA = 0 (start at offset 0)
     * FSE = 0x7F (127 elements, max for G0B1)
     */
    WRITE_REG(FDCAN1->RXGPC,
              (0U << 0)   |   /* FLSA */
              (0x7FU << 8));  /* FSE (127 filters) */

    /* Accept all standard IDs (filter 0: match all) */
    WRITE_REG(FDCAN1->RXFIS, 0U);

    /* Enable Rx FIFO 0 interrupts: new message */
    SET_BIT(FDCAN1->IR, 0x80000000UL);   /* RF0N - RX FIFO 0 new message */

    /* Enable FDCAN1_IT0 IRQ in NVIC */
    NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

    /* Exit configuration mode (CCCR.INIT = 0) */
    CLEAR_BIT(FDCAN1->CCCR, 0x1UL);
    while (READ_BIT(FDCAN1->CCCR, 0x1UL)) {}
}

int FDCAN1_SendMessage(const FDCAN_Message_t *msg)
{
    if (msg == NULL || msg->dlc > 8) {
        return -1;
    }

    /* Check if TX FIFO has space */
    if (READ_BIT(FDCAN1->TXFQS, 0xFFU)) {  /* TFQF - TX FIFO full */
        return -1;
    }

    /* Write to TX buffer (simplified: no CAN FD, standard ID only)
     * Message buffer format depends on FDCAN implementation details.
     * For now, this is a placeholder — actual message writing requires
     * understanding the exact TX buffer layout in the G0B1 reference manual.
     */
    /* TODO: Implement actual TX buffer write */

    return 0;
}

void FDCAN1_RegisterCallback(FDCAN_Callback_t cb)
{
    fdcan1_callback = cb;
}

int FDCAN1_IsReady(void)
{
    uint32_t psr = READ_REG(FDCAN1->PSR);
    /* Check if not in error passive or bus off */
    return !(psr & 0x60U);  /* EP and BO bits */
}

/* IRQ handler for FDCAN1_IT0 */
void FDCAN1_IT0_IRQHandler(void)
{
    /* Check Rx FIFO 0 new message flag */
    if (READ_BIT(FDCAN1->IR, 0x80000000UL)) {
        /* TODO: Extract message from Rx FIFO 0 and call callback */
        /* Clear flag */
        SET_BIT(FDCAN1->IR, 0x80000000UL);
    }
}
