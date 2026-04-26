/**
 * @file fdcan1.c
 * @brief FDCAN1 CAN 2.0 driver for STM32G0B1 @ 64 MHz.
 *
 * Pins : PB8 RX / PB9 TX (AF3)
 * Rate : 500 kbps nominal, classic CAN (not CAN FD)
 *
 * Message RAM layout (SRAMCAN_BASE + offset):
 *   0x000-0x02F  RX FIFO 0  — 3 elements × 4 words × 4 B = 48 B
 *   0x030-0x03F  TX buffer  — 1 element  × 4 words × 4 B = 16 B
 *
 * The layout is automatic; the hardware calculates start addresses
 * from filter list sizes in RXGFC (LSS=0, LSE=0 → no filters,
 * accept-all via ANFS/ANFE = 01).
 */
#include "fdcan1.h"
#include "fdcan_codec.h"
#include <stddef.h>

/* =========================================================== */
/*  Message RAM helpers                                        */
/* =========================================================== */

/* Word-pointer into the shared CAN message RAM */
#define SRAMCAN_WORD(byte_off) \
    ((volatile uint32_t *)(SRAMCAN_BASE + (byte_off)))

/* TX buffer 0 starts after RX FIFO 0:
 *   3 elements × FDCAN_ELEMENT_WORDS words × 4 B = 48 B  */
#define TX_BUF0_OFFSET   (3U * FDCAN_ELEMENT_WORDS * 4U)

/* =========================================================== */
/*  IR / IE / RXGFC / TXBC bit definitions                    */
/* =========================================================== */

#define FDCAN_IR_RF0N    (1UL << 0U)   /* RX FIFO 0 new message */
#define FDCAN_IE_RF0NE   (1UL << 0U)   /* Enable RF0N interrupt  */
#define FDCAN_ILE_EINT0  (1UL << 0U)   /* Enable interrupt line 0 */

/* RXGFC: accept non-matching std/ext frames into FIFO 0 (ANFS/ANFE = 01) */
#define FDCAN_RXGFC_ACCEPT_ALL  ((1UL << 4U) | (1UL << 6U))
/* RXGFC: F0S[21:16] = 3 (3 elements in RX FIFO 0) */
#define FDCAN_RXGFC_F0S_3       (3UL << 16U)

/* TXBC: NDTB[5:0] = 1 (one dedicated TX buffer) */
#define FDCAN_TXBC_NDTB_1       (1UL << 0U)

/* TXFQS: bit 21 = TX FIFO full flag */
#define FDCAN_TXFQS_TFQF        (1UL << 21U)

/* RXF0S: [5:0] fill level, [13:8] get index */
#define FDCAN_RXF0S_F0FL_Msk    0x3FUL
#define FDCAN_RXF0S_F0GI_Pos    8U
#define FDCAN_RXF0S_F0GI_Msk    (0x3FUL << FDCAN_RXF0S_F0GI_Pos)

/* =========================================================== */
/*  Module state                                               */
/* =========================================================== */

static FDCAN_Callback_t fdcan1_callback = NULL;

/* =========================================================== */
/*  Private: GPIO alternate-function configuration             */
/* =========================================================== */

static void gpio_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    /* Set alternate-function mode */
    MODIFY_REG(port->MODER,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    CLEAR_BIT(port->OTYPER,  (1UL << pin));          /* push-pull */
    MODIFY_REG(port->OSPEEDR,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));               /* high speed */
    CLEAR_BIT(port->PUPDR,   (0x3UL << (pin * 2U))); /* no pull */

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

/* =========================================================== */
/*  FDCAN1_Init                                                */
/* =========================================================== */

void FDCAN1_Init(void)
{
    /* 1. Clocks */
    SET_BIT(RCC->IOPENR,  RCC_IOPENR_GPIOBEN);
    SET_BIT(RCC->APBENR1, RCC_APBENR1_FDCAN1EN);

    /* 2. GPIO: PB8 = RX (AF3), PB9 = TX (AF3) */
    gpio_af(GPIOB, 8U, 3U);
    gpio_af(GPIOB, 9U, 3U);

    /* 3. Enter init + configuration mode */
    SET_BIT(FDCAN1->CCCR, 0x1UL);                       /* CCCR.INIT = 1 */
    while (!READ_BIT(FDCAN1->CCCR, 0x1UL)) {}
    SET_BIT(FDCAN1->CCCR, 0x2UL);                       /* CCCR.CCE  = 1 */

    /* 4. Nominal bitrate: 500 kbps at 64 MHz
     *   NBRP=7 → tq clock = 64 MHz / 8 = 8 MHz
     *   NTSEG1=11, NTSEG2=3 → 16 tq total → 500 kbps
     *   NSJW=3
     */
    WRITE_REG(FDCAN1->NBTP,
              (7U  <<  0U) |    /* NBRP    */
              (11U <<  8U) |    /* NTSEG1  */
              (3U  << 16U) |    /* NTSEG2  */
              (3U  << 24U));    /* NSJW    */

    /* 5. Message RAM: 3-element RX FIFO 0, accept all frames */
    WRITE_REG(FDCAN1->RXGFC,
              FDCAN_RXGFC_F0S_3 | FDCAN_RXGFC_ACCEPT_ALL);

    /* 6. TX buffer: 1 dedicated TX buffer, classic CAN (8-byte data) */
    WRITE_REG(FDCAN1->TXBC, FDCAN_TXBC_NDTB_1);
    WRITE_REG(FDCAN1->TXESC, 0U);   /* TBDS=0 → 8 bytes data field */

    /* 7. Interrupt routing: RF0N on line 0 → FDCAN1_IT0_IRQn */
    SET_BIT(FDCAN1->IE,  FDCAN_IE_RF0NE);
    WRITE_REG(FDCAN1->ILS, 0U);                          /* all on line 0 */
    SET_BIT(FDCAN1->ILE, FDCAN_ILE_EINT0);

    NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

    /* 8. Exit init mode */
    CLEAR_BIT(FDCAN1->CCCR, 0x2UL);                     /* CCCR.CCE  = 0 */
    CLEAR_BIT(FDCAN1->CCCR, 0x1UL);                     /* CCCR.INIT = 0 */
    while (READ_BIT(FDCAN1->CCCR, 0x1UL)) {}
}

/* =========================================================== */
/*  FDCAN1_SendMessage                                         */
/* =========================================================== */

int FDCAN1_SendMessage(const FDCAN_Message_t *msg)
{
    uint32_t element[FDCAN_ELEMENT_WORDS];

    if (msg == NULL || !FDCAN_Codec_IsClassicDlcValid(msg->dlc)) {
        return -1;
    }

    /* Check TX buffer 0 is not already pending */
    if (READ_BIT(FDCAN1->TXBRP, 0x1UL)) {
        return -1;   /* previous frame still queued */
    }

    /* Pack and write TX element to message RAM */
    FDCAN_Codec_PackTxElement(element, msg);

    volatile uint32_t *ram = SRAMCAN_WORD(TX_BUF0_OFFSET);
    ram[0] = element[0];
    ram[1] = element[1];
    ram[2] = element[2];
    ram[3] = element[3];

    /* Request transmission of buffer 0 */
    WRITE_REG(FDCAN1->TXBAR, 0x1UL);

    return 0;
}

/* =========================================================== */
/*  FDCAN1_RegisterCallback                                    */
/* =========================================================== */

void FDCAN1_RegisterCallback(FDCAN_Callback_t cb)
{
    fdcan1_callback = cb;
}

/* =========================================================== */
/*  FDCAN1_IsReady                                             */
/* =========================================================== */

int FDCAN1_IsReady(void)
{
    uint32_t psr = READ_REG(FDCAN1->PSR);
    return !(psr & (FDCAN_PSR_EP | FDCAN_PSR_BO));
}

/* =========================================================== */
/*  FDCAN1_IT0_IRQHandler                                      */
/*                                                             */
/*  Drains all pending messages from RX FIFO 0 and calls the  */
/*  registered callback for each frame.                        */
/* =========================================================== */

void FDCAN1_IT0_IRQHandler(void)
{
    if (!READ_BIT(FDCAN1->IR, FDCAN_IR_RF0N)) {
        return;
    }

    /* Clear RF0N flag (write 1 to clear) */
    WRITE_REG(FDCAN1->IR, FDCAN_IR_RF0N);

    /* Drain all available messages */
    uint32_t rxf0s = READ_REG(FDCAN1->RXF0S);
    uint32_t fill  = rxf0s & FDCAN_RXF0S_F0FL_Msk;
    uint32_t gi    = (rxf0s & FDCAN_RXF0S_F0GI_Msk) >> FDCAN_RXF0S_F0GI_Pos;

    while (fill > 0U) {
        /* Compute element address in SRAMCAN */
        uint32_t byte_off = gi * FDCAN_ELEMENT_WORDS * 4U;
        volatile uint32_t *ram = SRAMCAN_WORD(byte_off);

        /* Unpack and deliver */
        uint32_t element[FDCAN_ELEMENT_WORDS];
        element[0] = ram[0];
        element[1] = ram[1];
        element[2] = ram[2];
        element[3] = ram[3];

        FDCAN_Message_t rxmsg;
        FDCAN_Codec_UnpackRxElement(element, &rxmsg);

        /* Acknowledge element — advance FIFO get index */
        WRITE_REG(FDCAN1->RXF0A, gi & 0x3FU);

        if (fdcan1_callback != NULL) {
            fdcan1_callback(&rxmsg);
        }

        /* Advance */
        gi    = (gi + 1U) % 3U;   /* FIFO depth = 3 (matches F0S) */
        fill--;
    }
}
