/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

#ifndef __UART_IRQ_H__
#define __UART_IRQ_H__

#include "driver.h"

bool_t txInterruptEnable( driver_t* );
bool_t rxInterruptEnable( driver_t* );
void onTxTimeOutCallback( TimerHandle_t );
void onRxTimeOutCallback( TimerHandle_t );

driver_t *uartData[5];  //Puntero a las variables con los datos de la UART instanciada
//se usa para que cada callback sepa sobre que UART esta trabajando.

#endif
