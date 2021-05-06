/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "main.h"
#include "keys.h"
#include "user_tasks.h"
/*==================[definiciones de datos internos]=========================*/
const gpioMap_t leds_t[] = {LED2,LED3};
const gpioMap_t gpio_t[] = {GPIO3,GPIO1};
/*==================[definiciones de datos externos]=========================*/
extern QueueHandle_t cola_1;
/*==================[definiciones de funciones internas]=====================*/
void gpio_init(void)
{
    gpioInit( GPIO7, GPIO_OUTPUT );
    gpioInit( GPIO5, GPIO_OUTPUT );
    gpioInit( GPIO3, GPIO_OUTPUT );
    gpioInit( GPIO1, GPIO_OUTPUT );
}
/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea

void tarea_A( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	while( TRUE )
	{
		gpioWrite( LEDB , ON );
		vTaskDelay( MAX_RATE / 2 );
		xQueueSend( cola_1 , "LED ON" ,  portMAX_DELAY );
		gpioWrite( LEDB , OFF );

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}

void tarea_C( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	TickType_t xPeriodicity =  PRINT_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	char mensaje[MSG_LENGTH];

	while( TRUE )
	{
		xQueueReceive( cola_1 , &mensaje,  portMAX_DELAY );			// Esperamos dato para imprimir

		uartWriteString( USED_UART , mensaje );
		uartWriteString( USED_UART , "\r\n" );

		vTaskDelay( PRINT_RATE );
	}
}
/*==================[fin del archivo]========================================*/
