/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "object.h"
#include "main.h"
/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/
extern tObjeto obj_1;
extern tObjeto obj_2;
/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funciones]====================*/

void imprimir(char* mensaje)
{
	uartWriteString( USED_UART, mensaje );
}


void objeto_crear(tObjeto* objeto)
{
	objeto->cola  = xQueueCreate(N_QUEUE,sizeof(tMensaje));

	if( objeto->cola == NULL)
	{
		gpioWrite( LED_ERROR, ON );
		imprimir( MSG_ERROR_QUE );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}
}

void post(tObjeto* objeto,tMensaje mensaje)
{
	xQueueSend( objeto->cola, &mensaje, portMAX_DELAY );
}

void post_to_c(tMensaje mensaje)
{
	post( &obj_1 , mensaje );
}

void post_to_d(tMensaje mensaje)
{
	post( &obj_2 , mensaje );
}

void get(tObjeto* objeto,tMensaje* mensaje)
{
	xQueueReceive( objeto->cola, mensaje ,portMAX_DELAY );
}

void get_to_c(tMensaje* mensaje)
{
	get( &obj_1 , mensaje );
}

void get_to_d(tMensaje* mensaje)
{
	get( &obj_2 , mensaje );
}
