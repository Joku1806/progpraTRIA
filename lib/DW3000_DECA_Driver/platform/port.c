/*! ----------------------------------------------------------------------------
 * @file    port.c
 * @brief   HW specific definitions and functions for portability
 *
 * @attention
 *
 * Copyright 2016-2020 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include <Arduino.h>
#include <platform/port.h>


/****************************************************************************/ /**
 *
 *                              APP global variables
 *
 *******************************************************************************/
extern SPI_HandleTypeDef hspi1;

/****************************************************************************/ /**
 *
 *                  Port private variables and function prototypes
 *
 *******************************************************************************/
static volatile uint32_t signalResetDone;

/* DW IC IRQ handler definition. */
static port_dwic_isr_t port_dwic_isr = NULL;

/****************************************************************************/ /**
 *
 *                          DW IC port section
 *
 *******************************************************************************/

/* @fn      reset_DW IC
 * @brief   DW_RESET pin on DW IC has 2 functions
 *          In general it is output, but it also can be used to reset the digital
 *          part of DW IC by driving this pin low.
 *          Note, the DW_RESET pin should not be driven high externally.
 * */
void reset_DWIC(void) {
  pinMode(SPI_reset, OUTPUT);
  digitalWrite(SPI_reset, LOW);
  delayMicroseconds(100);
  pinMode(SPI_reset, INPUT);
  delayMicroseconds(1000);

  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
}

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
void wakeup_device_with_io(void) {
  SET_WAKEUP_PIN_IO_HIGH;
  WAIT_500uSEC;
  SET_WAKEUP_PIN_IO_LOW;
}

/* @fn      port_set_dw_ic_spi_slowrate
 * @brief   set 4.5MHz
 *          note: hspi1 is clocked from 72MHz
 * */
void port_set_dw_ic_spi_slowrate(void) {
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  HAL_SPI_Init(&hspi1);
}

/* @fn      port_set_dw_ic_spi_fastrate
 * @brief   set 18MHz
 *          note: hspi1 is clocked from 72MHz
 * */
void port_set_dw_ic_spi_fastrate(void) {
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  HAL_SPI_Init(&hspi1);
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn port_set_dwic_isr()
 *
 * @brief This function is used to install the handling function for DW IC IRQ.
 *
 * NOTE:
 *   - The user application shall ensure that a proper handler is set by calling this function before any DW IC IRQ occurs.
 *   - This function deactivates the DW IC IRQ line while the handler is installed.
 *
 * @param deca_isr function pointer to DW IC interrupt handler to install
 *
 * @return none
 */
void port_set_dwic_isr(port_dwic_isr_t dwic_isr) {
  /* Check DW IC IRQ activation status. */
  ITStatus en = port_GetEXT_IRQStatus();

  /* If needed, deactivate DW IC IRQ during the installation of the new handler. */
  port_DisableEXT_IRQ();

  port_dwic_isr = dwic_isr;

  if (!en) {
    port_EnableEXT_IRQ();
  }
}

/****************************************************************************/ /**
 *
 *                          End APP port section
 *
 *******************************************************************************/

/****************************************************************************/ /**
 *
 *                              IRQ section
 *
 *******************************************************************************/

/* @fn         HAL_GPIO_EXTI_Callback
 * @brief      EXTI line detection callback from HAL layer
 * @param      GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 *             i.e. DW_RESET_Pin and DW_IRQn_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
    case DW_RESET_Pin:
      signalResetDone = 1;
      break;
    case DW_IRQn_Pin:
      process_deca_irq();
      break;
  }
}

/* @fn      process_deca_irq
 * @brief   main call-back for processing of DW3000 IRQ
 *          it re-enters the IRQ routing and processes all events.
 *          After processing of all events, DW3000 will clear the IRQ line.
 * */
__INLINE void process_deca_irq(void) {
  while (port_CheckEXT_IRQ() != 0) {
    if (port_dwic_isr) {
      port_dwic_isr();
    }
  }
}

/* @fn      port_DisableEXT_IRQ
 * @brief   wrapper to disable DW_IRQ pin IRQ
 *          in current implementation it disables all IRQ from lines 5:9
 * */
__INLINE void port_DisableEXT_IRQ(void) {
  NVIC_DisableIRQ(DECAIRQ_EXTI_IRQn);
}

/* @fn      port_EnableEXT_IRQ
 * @brief   wrapper to enable DW_IRQ pin IRQ
 *          in current implementation it enables all IRQ from lines 5:9
 * */
__INLINE void port_EnableEXT_IRQ(void) {
  NVIC_EnableIRQ(DECAIRQ_EXTI_IRQn);
}

/**
  * @brief  Checks whether the specified IRQn line is enabled or not.
  * @param  IRQn: specifies the IRQn line to check.
  * @return "0" when IRQn is "not enabled" and !0 otherwise
  */
ITStatus EXTI_GetITEnStatus(IRQn_Type IRQn) {
  return ((NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] &
           (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) == (uint32_t)RESET)
             ? (RESET)
             : (SET);
}

/* @fn      port_GetEXT_IRQStatus
 * @brief   wrapper to read a DW_IRQ pin IRQ status
 * */
__INLINE uint32_t port_GetEXT_IRQStatus(void) {
  return EXTI_GetITEnStatus(DECAIRQ_EXTI_IRQn);
}

/* @fn      port_CheckEXT_IRQ
 * @brief   wrapper to read DW_IRQ input pin state
 * */
__INLINE uint32_t port_CheckEXT_IRQ(void) {
  return HAL_GPIO_ReadPin(DECAIRQ_GPIO, DW_IRQn_Pin);
}

/****************************************************************************/ /**
 *
 *                              END OF IRQ section
 *
 *******************************************************************************/