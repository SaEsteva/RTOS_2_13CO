/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

/*==================[ Inclusions ]============================================*/
#include "keys.h"
#include "main.h"
#include "object.h"
/*=====[ Definitions of private data types ]===================================*/
const t_key_config  keys_config[] = { TEC3,TEC4 };
#define KEY_COUNT   sizeof(keys_config)/sizeof(keys_config[0])
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Definitions of public global variables]==============================*/
t_key_data keys_data[KEY_COUNT];
extern tObjeto obj_1;
extern tObjeto obj_2;
/*=====[Implementations of public functions]=================================*/
TickType_t get_diff( uint32_t index )
{
	TickType_t tiempo;

	taskENTER_CRITICAL();
	tiempo = keys_data[index].time_diff;
	taskEXIT_CRITICAL();

	return tiempo;
}

void clear_diff( uint32_t index )
{
	taskENTER_CRITICAL();
	keys_data[index].time_diff = KEYS_INVALID_TIME;
	taskEXIT_CRITICAL();
}

void keys_Init( void )
{
	BaseType_t res;
	uint32_t i;

	for (i = 0 ; i < KEY_COUNT ; i++)
	{
		keys_data[i].state          = BUTTON_UP;  // Set initial state
		keys_data[i].time_down      = KEYS_INVALID_TIME;
		keys_data[i].time_up        = KEYS_INVALID_TIME;
		keys_data[i].time_diff      = KEYS_INVALID_TIME;
	}
	// Crear tareas en freeRTOS
	res = xTaskCreate (
	   tarea_B,					// Funcion de la tarea a ejecutar
	  ( const char * )"tarea_B",	// Nombre de la tarea como String amigable para el usuario
	  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
	  0,							// Parametros de tarea
	  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
	  0							// Puntero a la tarea creada en el sistema
	);

	// Gestión de errores
	configASSERT( res == pdPASS );
}

// keys_ Update State Function
void keys_Update( uint32_t index )
{
	switch( keys_data[index].state )
	{
		case STATE_BUTTON_UP:
			/* CHECK TRANSITION CONDITIONS */
			if( !gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_FALLING;
			}
			break;

		case STATE_BUTTON_FALLING:
			/* ENTRY */

			/* CHECK TRANSITION CONDITIONS */
			if( !gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_DOWN;

				/* ACCION DEL EVENTO !*/
				buttonPressed( index );
			}
			else
			{
				keys_data[index].state = STATE_BUTTON_UP;
			}

			/* LEAVE */
			break;

		case STATE_BUTTON_DOWN:
			/* CHECK TRANSITION CONDITIONS */
			if( gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_RISING;
			}
			break;

		case STATE_BUTTON_RISING:
			/* ENTRY */

			/* CHECK TRANSITION CONDITIONS */

			if( gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_UP;

				/* ACCION DEL EVENTO ! */
				buttonReleased( index );
			}
			else
			{
				keys_data[index].state = STATE_BUTTON_DOWN;
			}

			/* LEAVE */
			break;

		default:
			keys_ButtonError( index );
			break;
	}
}

/*=====[Implementations of private functions]================================*/

/* accion de el evento de tecla pulsada */
static void buttonPressed( uint32_t index )
{
	TickType_t current_tick_count = xTaskGetTickCount();

	taskENTER_CRITICAL();
	keys_data[index].time_down = current_tick_count;
	taskEXIT_CRITICAL();
}

/* accion de el evento de tecla liberada */
static void buttonReleased( uint32_t index )
{
	TickType_t current_tick_count = xTaskGetTickCount();

	tPrint data_to_send;

	taskENTER_CRITICAL();
	keys_data[index].time_up    = current_tick_count;
	keys_data[index].time_diff  = keys_data[index].time_up - keys_data[index].time_down;
	keys_data[index].tecla = index+keys_config[0].tecla;
	taskEXIT_CRITICAL();

	if (keys_data[index].time_diff > 0)
	{
		tMensaje mensaje;
		mensaje = pvPortMalloc(sizeof(FSM_MSG));

		if (mensaje != NULL)
		{
			sprintf (mensaje, FSM_MSG , btn_format(keys_data[index].tecla),time_format(keys_data[index].time_diff));
			//xQueueSend( cola_1 , &mensaje ,  portMAX_DELAY );
			//post(&obj_1,mensaje);
			post_to_c(mensaje);
		}
		else
		{
			printf(MALLOC_ERROR);
		}

	}
}

static void keys_ButtonError( uint32_t index )
{
	taskENTER_CRITICAL();
	keys_data[index].state = BUTTON_UP;
	taskEXIT_CRITICAL();
}

/*=====[Implementations of private functions]=================================*/
void tarea_B( void* taskParmPtr )
{
	uint32_t i;
	while( TRUE )
	{
		for (i = 0 ; i < KEY_COUNT ; i++)
			keys_Update( i );
		vTaskDelay( DEBOUNCE_TIME_MS );
	}
}
