/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"
#include "mayusc.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

// Instanciamos un tipo de dato para comunicar la aplicación con los Objetos Activos.
activeObjectResponse_t packetResponse;

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

void tickTask( void* pvParameters )
{
    while( TRUE )
    {
        // Una tarea muy bloqueante para demostrar que la interrupcion funcina
        gpioToggle( LEDG );
        vTaskDelay( 100/portTICK_RATE_MS );
    }
}

void tarea_auxiliar( void* pvParameters )
{
	driver_t *selectedUart;
	selectedUart = ( driver_t * )pvParameters;
	activeObjectResponse_t auxResponse;

	while( TRUE )
	{
		gpioToggle( LED3 );
		if( xQueueReceive( packetResponse.colaCapa3, &auxResponse , 0 ) ) // portMAX_DELAY
			packetTX( selectedUart,auxResponse.string );
		vTaskDelay( 100/portTICK_RATE_MS );
	}

}
void activeObjectDispatch( void* pvParameters )
{
	driver_t *selectedUart;
	selectedUart = ( driver_t * )pvParameters;

	char *TXbuffer;
    char evento;

    activeObject_t normalAO;
    normalAO.itIsAlive = FALSE;

    activeObject_t sietizadorAO;
    sietizadorAO.itIsAlive = FALSE;

	while( TRUE )
	{
		gpioToggle( TASK_LED ); //Led para debug

		TXbuffer = waitPacket( selectedUart ); //espero un paquete
		if ( TXbuffer!=NULL )	//si recibo null es porque ocurrio un error en la comunicacion
		{
			packetResponse.string = TXbuffer;
			evento = getCommand( TXbuffer );
			if( evento != EVENT )
			{
				if( normalAO.itIsAlive == FALSE )
				{
					// Se crea el objeto activo, con el comando correspondiente y tarea asociada.
					activeObjectCreate( &normalAO, Check_packet, activeObjectTask );
				}

				// Y enviamos el dato a la cola para procesar.
				activeObjectEnqueue( &normalAO, &packetResponse );
				//packetTX( selectedUart,packetResponse.string );
			}
			if( evento == EVENT )
			{
				if( sietizadorAO.itIsAlive == FALSE )
				{
					// Se crea el objeto activo, con el comando correspondiente y tarea asociada.
					activeObjectCreate( &sietizadorAO, Sietizador, activeObjectTask );
				}

				// Y enviamos el dato a la cola para procesar.
				activeObjectEnqueue( &sietizadorAO, &packetResponse );
				//packetTX( selectedUart,packetResponse.string );
			}
		}
	}
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

