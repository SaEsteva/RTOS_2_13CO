/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

#include "driver.h"
#include "FreeRTOSConfig.h"
#include "userTasks.h"

#define UART UART_USB
#define BAUDRATE 115200
#define ERROR_LED LEDR
#define DRIVER_ERROR_MSG "Error al inicializar el driver.\r\n"
#define TASK_ERROR_MSG "Error al crear las tareas.\r\n"

int main( void )
{
    static driver_t selectedUart; //Variable de la instanciacion del driver que va a usar la tarea principal

    /* Inicializar la placa */

    boardConfig();

    //Inicializacion del driver de UART

    selectedUart.uartValue = UART;
    selectedUart.baudRate = BAUDRATE;
    if ( driverInit( &selectedUart ) == FALSE )
    {
        gpioWrite( ERROR_LED ,TRUE ); //Error al inicializar el driver
        printf( DRIVER_ERROR_MSG );
        while( TRUE );
    }

    BaseType_t res_A = xTaskCreate(
        tickTask,                     // Funcion de la tarea a ejecutar
        ( const char * )"tickTask",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        0,                          // Parametros de tarea
        tskIDLE_PRIORITY+1,         // Prioridad de la tarea
        0                           // Puntero a la tarea creada en el sistema
    );

    BaseType_t res_B = xTaskCreate(
    	checkTask,                     // Funcion de la tarea a ejecutar
        ( const char * )"tarea de chequeo de paquetes",   // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
        ( void* )&selectedUart,                        // Parametros de tarea
        tskIDLE_PRIORITY+2,         // Prioridad de la tarea
        0                           // Puntero a la tarea creada en el sistema
    );


    // Gestion de errores
	if(!(res_A && res_B ))
	{
		gpioWrite( ERROR_LED ,TRUE ); //Error al inicializar tareas
		printf( TASK_ERROR_MSG );
		while( TRUE );	// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

    // Iniciar scheduler
    vTaskStartScheduler(); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    while(TRUE); // Nunca debe llegar aca!

    return TRUE;
}
