#ifndef __TEST_UART_H__
#define __TEST_UART_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "misc.h"
#include "string.h"

void uart1_init(uint32_t bound_rate);
void uart2_init(uint32_t bound_rate);
void uart3_init(uint32_t bound_rate);

#endif

