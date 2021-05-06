/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

#ifndef KEYS_H_
#define KEYS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"

/* public macros ================================================================= */
#define KEYS_INVALID_TIME   -1
#define DEBOUNCE_TIME   40
#define DEBOUNCE_TIME_MS pdMS_TO_TICKS(DEBOUNCE_TIME)
#define FSM_MSG "TEC%01d T%04u ms\r\n"
#define MSG_LENGTH 20
#define btn_format(x) 			x-TEC1+1
#define time_format(x) 			x*portTICK_RATE_MS
/* types ================================================================= */
typedef char* tMensaje;

typedef enum
{
	STATE_BUTTON_UP,
	STATE_BUTTON_DOWN,
	STATE_BUTTON_FALLING,
	STATE_BUTTON_RISING
} keys_ButtonState_t;

typedef struct
{
	gpioMap_t tecla;			//config
} t_key_config;

typedef struct
{
	gpioMap_t tecla;				//config
	TickType_t time_diff;	//variables
} tPrint;

typedef struct
{
	keys_ButtonState_t state;   //variables
	gpioMap_t tecla;

	TickType_t time_down;		//timestamp of the last High to Low transition of the key
	TickType_t time_up;		    //timestamp of the last Low to High transition of the key
	TickType_t time_diff;	    //variables
} t_key_data;

/*=====[Prototypes (declarations) of private functions]======================*/

static void keys_ButtonError( uint32_t index );
static void buttonPressed( uint32_t index );
static void buttonReleased( uint32_t index );

/* methods ================================================================= */
void keys_Init( void );
TickType_t get_diff(uint32_t index);
void clear_diff(uint32_t index);

/*=====[prototype of private functions]=================================*/
void tarea_B( void* taskParmPtr );

#endif /* KEYS_H_ */
