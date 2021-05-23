/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"
#include "mayusc.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

void tickTask( void* pvParameters )
{
    while( TRUE )
    {
        // Una tarea muy bloqueante para demostrar que la interrupcion funcina
        gpioToggle( TICK_LED );
        vTaskDelay( TASK_DELAY );
    }
}

//capTask recibe como parametro los datos de una UART con el driver inicializado
//se queda esperando a que llegue un paquete, lo procesa y lo envia por la UART
void checkTask( void* pvParameters )
{
    char *TXbuffer;
    driver_t *Uart_driver;
    Uart_driver = ( driver_t * )pvParameters;
    while ( TRUE )
    {
    	gpioToggle( CHECK_LED );

        TXbuffer = waitPacket( Uart_driver ); //espero un paquete
        if ( TXbuffer!=NULL )	//si recibo null es porque ocurrio un error en la comunicacion
        {
            Check_packet( TXbuffer ); //Analizo si el paquete es valido y luego lo paso a mayuscula si corresponde o cargo un mensaje de error
            packetTX( Uart_driver,TXbuffer ); //envio el paquete procesado o el mensaje de error.
        }

        vTaskDelay( TASK_DELAY );
    }
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

