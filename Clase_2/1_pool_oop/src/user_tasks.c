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
#include "object.h"

#include "qmpool.h"
/*==================[definiciones de datos internos]=========================*/
const gpioMap_t leds_t[] = {LED2,LED3};
const gpioMap_t gpio_t[] = {GPIO3,GPIO1};
tObjeto obj_1;
tObjeto obj_2;
/*==================[definiciones de datos externos]=========================*/
extern tMensaje Pool_puntero;
extern QMPool Pool_memoria;
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

	tMensaje mensaje;

	while( TRUE )
	{
		gpioWrite( LEDB , ON );
		vTaskDelay( MAX_RATE / 2 );

		//mensaje = pvPortMalloc(sizeof(MSG_LED));

		// Asigno un bloque de memoria
		mensaje = ( tMensaje ) QMPool_get( &Pool_memoria, 0 ); //pido un bloque del pool

		if (mensaje != NULL)
		{
			strcpy(mensaje,MSG_LED);
			//xQueueSend( cola_1 , &mensaje ,  portMAX_DELAY );
			//post(&obj_1,mensaje);
			post_to_c(mensaje);
		}
		else
		{
			printf(MALLOC_ERROR);
		}

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

	tMensaje mensaje;

	while( TRUE )
	{
		//xQueueReceive( cola_1 , &p ,portMAX_DELAY );
		//get(&obj_1,&mensaje);
		get_to_c(&mensaje);

		imprimir( mensaje );

		//post(&obj_2,mensaje);
		post_to_d(mensaje);

		vTaskDelay( PRINT_RATE );
	}
}


void tarea_D( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
    TickType_t xPeriodicity =  MAX_RATE;		// Tarea periodica cada 1000 ms
    TickType_t xLastWakeTime = xTaskGetTickCount();

    tMensaje mensaje;

    // ---------- REPETIR POR SIEMPRE --------------------------

    while( TRUE )
    {
    	//get(&obj_2,&mensaje);
    	get_to_d(&mensaje);

    	//vPortFree(mensaje);

		// Libero el bloque de memoria que ya fue trasmitido
		QMPool_put( &Pool_memoria, mensaje );
    	mensaje = NULL;

        vTaskDelay( xPeriodicity / 20 );
    }
}

/*==================[fin del archivo]========================================*/
