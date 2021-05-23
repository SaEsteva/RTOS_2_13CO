/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "driver.h"
#include "uartIRQ.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

// Inicializacion de parte de transmision del driver
// Requiere que se le pase una variable del tipo driverConfig_t
// con los siguientes datos:
// 	selectedUart.uartValue = (por ej:UART_USB);
//	selectedUart.baudRate = (por ej: 115200);
bool_t driverInit( driver_t* selectedUart )
{
    // Inicializamos la UART que se ha seleccionado y con el baud rate especificado.
    uartConfig( selectedUart->uartValue, selectedUart->baudRate );

    // Creamos el timer que cumplira la funcion de time out y poder separar los paquetes.

    selectedUart->onTxTimeOut = xTimerCreate( "TX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) selectedUart, onTxTimeOutCallback );
    selectedUart->onRxTimeOut = xTimerCreate( "RX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) selectedUart, onRxTimeOutCallback );

    // Creamos la cola para señalizar la recepcion de un dato valido hacia la aplicacion.
    selectedUart->onRxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

    //	Creo una cola donde van a ir los bloque que tengo que mandar por UART
    selectedUart->onTxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) ); //La cola va a tener tantos elementos como bloques de memoria pueda tener el pool

    //	Reservo memoria para el memory pool
    selectedUart->txPoolSto = ( char* ) pvPortMalloc( POOL_SIZE * sizeof( char ) );

    //	Creo el pool de memoria que va a usarse para la transmision
    QMPool_init( &selectedUart->txPool, ( void* ) selectedUart->txPoolSto,POOL_SIZE * sizeof( char ),BLOCK_SIZE );
    //         (-------- Pool --------,------ Memoria para el pool ------,---- Cant. de memoria ----,--N Bloques--);

    //	Validamos si todos los recursos que requeran de memoria fueron creados exitosamente.
    if ( ( selectedUart->onRxTimeOut == NULL ) ||		// timer de RX
            ( selectedUart->onTxTimeOut == NULL ) ||	// timer de TX
            ( selectedUart->onRxQueue == NULL ) ||      // cola para RX
            ( selectedUart->onTxQueue == NULL ) ||      // cola para TX
            ( selectedUart->txPoolSto == NULL ) )       // Memoria pedida para el pool
    {
        // No hay memoria suficiente. Por este motivo, retornamos false.
        return( FALSE );
    }


    selectedUart->txCounter = 0; // Inicializo el contador de bytes enviados
    selectedUart->rxLen = 0;	 // Inicializo el contador de tamaño RX
    selectedUart->txLen = 0;	 // Inicializo el contador de tamaño TX
    selectedUart->rxChecksum =0; // Inicializo el acumulador del crc

    // Asigno un bloque de memoria para la primera recepcion de la UART
    selectedUart->rxBlock = ( char* ) QMPool_get( &selectedUart->txPool , 0 ); //pido un bloque del pool

    // Configuramos el callback del evento de recepciÃ³n y habilitamos su interrupciÃ³n.
    if( ( rxInterruptEnable( selectedUart ) ) )
    {}
    else  //se eligio una UART no valida
    {
        return ( FALSE );
    }

    // Habilitamos todas las interrupciones de la UART seleccionada.
    uartInterrupt( selectedUart->uartValue, TRUE );

    // Si todos los elementos fueron creados e inicializados correctamente, indicamos que el driver estÃ¡ listo para ser utilizado.
    return( TRUE );
}

//	packetTX(driverConfig_t* selectedUart, char *txBlock) transmite el bloque
//	txBlock por la UART indicada por selectedUart
void packetTX( driver_t* selectedUart, char *txBlock )
{

    xQueueSend( selectedUart->onTxQueue, &txBlock, portMAX_DELAY ); //Envio a la cola de transmision el blocke a transmitir
    taskENTER_CRITICAL();  //no permito que se modifique txcounter
    if ( selectedUart->txCounter == 0 ) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
    {
        txInterruptEnable( selectedUart );
    }
    taskEXIT_CRITICAL();
    uartSetPendingInterrupt( selectedUart->uartValue );
}


// waitPacket() se queda esperando un paquete de la UART seleccionada y devuelve un puntero al bloque de memoria
char* waitPacket( driver_t* selectedUart ) //  == get(&objeto)
{
    char*dato_rx;
    xQueueReceive( selectedUart->onRxQueue,( void * )&dato_rx,portMAX_DELAY ); //espero a que venga un bloque por la cola
    return dato_rx;
}

/*=====[Implementations of private functions]================================*/


