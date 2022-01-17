#pragma once

#include <decadriver/deca_device_api.h>

void DWIC_reset();
void DWIC_configure_spi(size_t spi_rate);
void DWIC_configure_interrupts(void (*recv_handler)(const dwt_cb_data_t *cb_data));
