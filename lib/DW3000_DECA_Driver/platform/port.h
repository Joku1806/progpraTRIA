/*! ----------------------------------------------------------------------------
 * @file    port.h
 * @brief   HW specific definitions and functions for portability
 *
 * @attention
 *
 * Copyright 2015-2020 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#ifndef PORT_H_
#define PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
//#include <compiler.h>

#include <platform/feather_m0.h>

/* DW IC IRQ (EXTI15_10_IRQ) handler type. */
typedef void (*port_dwic_isr_t)(void);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn port_set_DWIC_isr()
 *
 * @brief This function is used to install the handling function for DW1000 IRQ.
 *
 * NOTE:
 *   - As EXTI9_5_IRQHandler does not check that port_deca_isr is not null, the user application must ensure that a
 *     proper handler is set by calling this function before any DW1000 IRQ occurs!
 *   - This function makes sure the DW1000 IRQ line is deactivated while the handler is installed.
 *
 * @param deca_isr function pointer to DW1000 interrupt handler to install
 *
 * @return none
 */
void port_set_dwic_isr(port_dwic_isr_t isr);

/****************************************************************************/ /**
 *
 *                              MACRO
 *
 *******************************************************************************/

#define DW_RSTn DW_RESET_Pin
#define DW_RSTn_GPIO DW_RESET_GPIO_Port

#define DECAIRQ DW_IRQn_Pin
#define DECAIRQ_GPIO DW_IRQn_GPIO_Port
#define DECAIRQ_EXTI_IRQn (EXTI15_10_IRQn)

/****************************************************************************/ /**
 *
 *                              MACRO function
 *
 *******************************************************************************/

#define usleep(usec) delayMicroseconds(usec)
#define Sleep(msec) delay(msec)

/****************************************************************************/ /**
 *
 *                              port function prototypes
 *
 *******************************************************************************/

void port_set_dw_ic_spi_slowrate(void);
void port_set_dw_ic_spi_fastrate(void);

void process_dwRSTn_irq(void);
void process_deca_irq(void);

void setup_DWICRSTnIRQ(int enable);

void reset_DWIC(void);

// Mutex Zeug
ITStatus EXTI_GetITEnStatus(IRQn_Type x);
uint32_t port_GetEXT_IRQStatus(void);
uint32_t port_CheckEXT_IRQ(void);
void port_DisableEXT_IRQ(void);
void port_EnableEXT_IRQ(void);

/*! ------------------------------------------------------------------------------------------------------------------
* @fn wakeup_device_with_io()
*
* @brief This function wakes up the device by toggling io with a delay.
*
* input None
*
* output -None
*
*/
void wakeup_device_with_io(void);

//This set the IO for waking up the chip
#define SET_WAKEUP_PIN_IO_LOW HAL_GPIO_WritePin(DW_WAKEUP_GPIO_Port, DW_WAKEUP_Pin, GPIO_PIN_RESET)
#define SET_WAKEUP_PIN_IO_HIGH HAL_GPIO_WritePin(DW_WAKEUP_GPIO_Port, DW_WAKEUP_Pin, GPIO_PIN_SET)

#define WAIT_500uSEC Sleep(1) /*This is should be a delay of 500uSec at least. In our example it is more than that*/

#ifdef __cplusplus
}
#endif

#endif /* PORT_H_ */