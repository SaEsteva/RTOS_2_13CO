/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

#include "AO.h"
#include "driver.h"
#include "FreeRTOSConfig.h"
#include "userTasks.h"

#define UART UART_USB
#define BAUDRATE 115200
#define ERROR_LED LEDR
#define N_QUEUE 	10
#define DRIVER_ERROR_MSG "Error al inicializar el driver.\r\n"
#define TASK_ERROR_MSG "Error al crear las tareas.\r\n"
#define QUEUE_ERROR_MSG "Error al crear la cola.\r\n"

extern activeObjectResponse_t packetResponse;

int main( void )
{
    static driver_t selectedUart; //Variable de la instanciacion del driver que va a usar la tarea principal
    BaseType_t res;

    boardConfig();	/* Inicializar la placa */

    //Inicializacion del driver de UART

    selectedUart.uartValue = UART;
    selectedUart.baudRate = BAUDRATE;
    if ( driverInit( &selectedUart ) == FALSE )
    {
        gpioWrite( ERROR_LED ,TRUE ); //Error al inicializar el driver
        printf( DRIVER_ERROR_MSG );
        while( TRUE );
    }

    res = xTaskCreate( tickTask ,(const char*)"tickTask",2*configMINIMAL_STACK_SIZE,0,tskIDLE_PRIORITY+1,0 );
    configASSERT( res == pdPASS );

    res = xTaskCreate( activeObjectDispatch ,(const char*)"tarea de chequeo de paquetes",2*configMINIMAL_STACK_SIZE,(void*)&selectedUart,tskIDLE_PRIORITY+2,0 );
    configASSERT( res == pdPASS );

    res = xTaskCreate( tarea_auxiliar ,(const char*)"tarea de chequeo de paquetes",2*configMINIMAL_STACK_SIZE,(void*)&selectedUart,tskIDLE_PRIORITY+1,0 );
    configASSERT( res == pdPASS );

	// Creamos la cola.
	packetResponse.colaCapa3 = xQueueCreate( N_QUEUE, sizeof( activeObjectResponse_t ) );
	configASSERT( packetResponse.colaCapa3 !=  NULL );

    // Iniciar scheduler
    vTaskStartScheduler(); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    while(TRUE); // Nunca debe llegar aca!

    return TRUE;
}
