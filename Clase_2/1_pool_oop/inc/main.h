/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

#ifndef MAIN_H_
#define MAIN_H_

/*==================[inclusiones]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
/*==================[definiciones y macros]==================================*/
#define RATE 					1000
#define PRINT_RATE_MS 			500
#define MAX_RATE 				pdMS_TO_TICKS(RATE)
#define PRINT_RATE 				pdMS_TO_TICKS(PRINT_RATE_MS)
#define USED_UART 				UART_USB
#define UART_RATE 				115200
#define WELCOME_MSG  			"Memory pool + \"OOP\".\r\n"
#define MALLOC_ERROR 			"Malloc Failed!\n"
#define MSG_ERROR_QUE 			"Queue error.\r\n"
#define MSG_ERROR_MTX 			"Mutex error.\r\n"
#define MSG_PRINT_FORMAT 		"You pressed TEC%d for %d ms\r\n"
#define MSG_LED					"LEDB ON\r\n"
#define LED_ERROR 				LEDR
#define N_QUEUE 				8
#define GPIO_PRINT   			GPIO0

#define PACKET_SIZE  50							// Tamanio del paquete
#define POOL_TOTAL_BLOCKS 10						// Cuantos paquetes
#define POOL_SIZE POOL_TOTAL_BLOCKS*PACKET_SIZE //Tamanio total del pool
/*==================[declaraciones de funciones externas]====================*/
TickType_t get_diff();
void clear_diff();

// Prototipo de funcion de la tarea
void tarea_led( void* taskParmPtr );
void tarea_print( void* taskParmPtr );

#endif /* MAIN_H_ */
