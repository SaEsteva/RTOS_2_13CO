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
/*==================[definiciones de datos internos]=========================*/
extern tObjeto obj_1;
extern tObjeto obj_2;
/*==================[definiciones de datos externos]=========================*/
DEBUG_PRINT_ENABLE;
#define LED_COUNT   sizeof(leds_t)/sizeof(leds_t[0])
/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------
	boardConfig();									// Inicializar y configurar la plataforma

	gpio_init();

	debugPrintConfigUart( USED_UART , UART_RATE );		// UART for debug messages
	printf( WELCOME_MSG );

	BaseType_t res;
	uint32_t i;

    // Crear tarea en freeRTOS
	res = xTaskCreate(
		tarea_A,                     // Funcion de la tarea a ejecutar
		( const char * )"tarea_A",   // Nombre de la tarea como String amigable para el usuario
		configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
		0,                          // Parametros de tarea
		tskIDLE_PRIORITY+1,         // Prioridad de la tarea
		0                           // Puntero a la tarea creada en el sistema
	);

	// Gestion de errores
	configASSERT( res == pdPASS );

	// Creo tarea unica de impresion
	res = xTaskCreate(
		tarea_C,                     // Funcion de la tarea a ejecutar
		( const char * )"tarea_C",   // Nombre de la tarea como String amigable para el usuario
		configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
		0,                          // Parametros de tarea
		tskIDLE_PRIORITY+1,         // Prioridad de la tarea
		0                           // Puntero a la tarea creada en el sistema
	);

	// Gestion de errores
	configASSERT( res == pdPASS );

	// Creo tarea unica de impresion
	res = xTaskCreate(
		tarea_D,                     // Funcion de la tarea a ejecutar
		( const char * )"tarea_D",   // Nombre de la tarea como String amigable para el usuario
		configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
		0,                          // Parametros de tarea
		tskIDLE_PRIORITY+1,         // Prioridad de la tarea
		0                           // Puntero a la tarea creada en el sistema
	);

	// Gestion de errores
	configASSERT( res == pdPASS );

    keys_Init();			 				// inicializo driver de teclas

    // Creacion Y validacion de objetos
	objeto_crear(&obj_1);
	objeto_crear(&obj_2);

    // Iniciar scheduler
    vTaskStartScheduler();					// Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    // ---------- REPETIR POR SIEMPRE --------------------------
    configASSERT( 0 );

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return TRUE;
}

/* hook que se ejecuta si al necesitar un objeto dinamico, no hay memoria disponible */
void vApplicationMallocFailedHook()
{
	printf( "Malloc Failed Hook!\n" );
	configASSERT( 0 );
}
/*==================[fin del archivo]========================================*/
