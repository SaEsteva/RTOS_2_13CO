/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/
#include "uartIRQ.h"

void onTxCallback( void* );
void onRxCallback( void * );

//	onTxTimeOutCallback
// Timer para la separacion de paquetes

void onTxTimeOutCallback( TimerHandle_t xTimer )
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    // Obtenemos los datos de la UART seleccionada, aprovechando el campo reservado para el Timer ID.
    driver_t* selectedUart = ( driver_t* ) pvTimerGetTimerID( xTimer );

    //Inicio seccion critica
    taskENTER_CRITICAL();

    uartTxWrite( selectedUart->uartValue,'\r' );
    //uartTxWrite( selectedUart->uartValue,'\n' );

    // Libero el bloque de memoria que ya fue trasmitido
    QMPool_put( &selectedUart->txPool, selectedUart->txBlock );

    selectedUart->txCounter = 0; //Reinicio el contador de bytes transmitidos para la siguiente transmision

    taskEXIT_CRITICAL(); // Fin de seccion critica

    //si hay mensajes esperando vuelvo a habilitar la interrupción y la disparo
    if ( uxQueueMessagesWaiting( selectedUart->onTxQueue ) )
    {
        txInterruptEnable( selectedUart );
        uartSetPendingInterrupt( selectedUart->uartValue );
    }

}

// Callback para el timeout definido en PROTOCOL_TIMEOUT.
void onRxTimeOutCallback( TimerHandle_t xTimer )
{
    int crcTemp;

    // Obtenemos los datos de la UART, aprovechando el campo reservado para el Timer ID.
    driver_t* selectedUart = ( driver_t* ) pvTimerGetTimerID( xTimer );

    // Inicio seccion critica
    taskENTER_CRITICAL();

    if ( selectedUart->rxLen <= PACKET_SIZE ) //si la cantidad de bytes es correcta
    {

    	selectedUart->rxBlock[0] = selectedUart->rxLen;
    	selectedUart->rxChecksum = str_to_checksum(selectedUart->rxBlock);

        // Si el CRC es cero, significa que el paquete es válido.
        if ( selectedUart->rxChecksum == (selectedUart->rxBlock[selectedUart->rxLen-1]-'0') )
        {
            // Se escribe en la cola el buffer completo y a su vez avisamos a la aplicación de que un paquete correcto se debe procesar.
            xQueueSend( selectedUart->onRxQueue,( void* )&( selectedUart->rxBlock ), portMAX_DELAY );

            //Una vez que el bloque esta en la cola pido otro bloque para el siguiente paquete. Es responsabilidad de
            //la aplicacion liberar el bloque mediante una transmision o con la funcion putBuffer()
            selectedUart->rxBlock = ( char* ) QMPool_get( &selectedUart->txPool,0 ); //pido un bloque del pool

            //Chequeo si tengo un bloque de memoria, sino anulo la recepcion de paquetes
            if ( selectedUart->rxBlock == NULL )
            {
                uartCallbackClr( selectedUart->uartValue, UART_RECEIVE );
            }
        }
        else
		{
			selectedUart->rxLen = 0;    //pongo el contador n cero y me quedo con el bloque de memoria
			// Aca podriamos poner alguna cosa para avisar el error de checksum
		}

        // Caso contrario, el paquete es inválido y se debe descartar (no necesito hacer nada)
    }
    else
    {
        selectedUart->rxLen = 0;    //pongo el contador n cero y me quedo con el bloque de memoria
    }

    // Reiniciamos el índice para cuando llegue un nuevo paquete.
    selectedUart->rxLen = 0;
    selectedUart->rxChecksum = 0; //Borro el acumulador de CRC para el siguiente paquete

    taskEXIT_CRITICAL(); // Salgo de la zona critica
}

//rxInterruptEnable(driverConfig_t* selectedUart) Habilita la interrupcion de la UART para recepcion
//indicada por selectedUart
bool_t rxInterruptEnable( driver_t* selectedUart )
{
    switch ( selectedUart->uartValue )
    {
        case UART_GPIO:
        case UART_USB:
        case UART_232:
        {
            uartCallbackSet( selectedUart->uartValue, UART_RECEIVE, onRxCallback,( void* ) selectedUart );
            break;
        }
        default:
        {
            // No se ha seleccionado una UART correcta, por lo que retornamos false como un error.
            return ( false );
        }
    }

    return ( true );
}

//txInterruptEnable(driverConfig_t* selectedUart) Habilita la interrupcion de la UART para transmision
//indicada por selectedUart
bool_t txInterruptEnable( driver_t* selectedUart )
{
    switch ( selectedUart->uartValue )
    {
        case UART_GPIO:
        case UART_USB:
        case UART_232:
        {
            uartCallbackSet( selectedUart->uartValue, UART_TRANSMITER_FREE,onTxCallback, ( void* ) selectedUart );
            break;
        }
        default:
        {
            // No se ha seleccionado una UART correcta, por lo que retornamos false como un error.
            return ( false );
        }
    }

    return ( true );
}

/*=====[Implementations of interrupt functions]==============================*/

// Callback para la recepción
void onRxCallback( void *param )
{
    UBaseType_t uxSavedInterruptStatus;
    BaseType_t xTaskWokenByReceive= pdFALSE; //Variable para evaluar el cambio de contexto al finaliza la interrupcion

    //Elijo los datos correspondientes al Callback
    driver_t* selectedUart = param;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();	// Protejo selectedUart
    //si llegan mas bytes de lo que entra en el buffer no los leo. Luego descarto el paquete en el timer de timeout
    if ( selectedUart->rxLen < PACKET_SIZE )
    {
        // Obtenemos el byte de la UART seleccionada y reservo el primer byte para guardar la longitud del string
        selectedUart->rxBlock[selectedUart->rxLen + 1] = uartRxRead( UART_USB );
        //selectedUart->rxCrc = crc8_calc( selectedUart->rxCrc,&selectedUart->rxBlock[selectedUart->rxLen + 1], 1 ); //acumulo el crc en cada byte que recibo
    }
    else
    {
        selectedUart->rxLen = PACKET_SIZE + 1; //limito el valor para no desbordar la variable
    }

    // Inicializamos el timer.
    xTimerStartFromISR( selectedUart->onRxTimeOut, &xTaskWokenByReceive );

    // Incrementamos el valor del índice.
    selectedUart->rxLen++;
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

    //Hago la evaluacion de cambio de cambio de contexto necesario.
    if ( xTaskWokenByReceive != pdFALSE )
    {
        taskYIELD ();
    }

}


//	uartSendDataCRC transmite los bloques que llegan por una cola y los envia por la UART seleccionada
//	Todas las callback de interrupcion de UART llaman a esta función para hacer la transmisión.
void onTxCallback( void*param )
{
    driver_t *selectedUart = ( driver_t * ) param;
    BaseType_t xTaskWokenByReceive = pdFALSE; //Variable para evaluar el cambio de contexto al finaliza la interrupcion
    UBaseType_t uxSavedInterruptStatus;

    //Armo una seccion crtitica para proteger las variables de txCounter
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    //	Si el contador esta en cero tengo que tomar un bloque de la cola, calcular CRC
    if ( selectedUart->txCounter == 0 )
    {
        //	Tomo el bloque a transmitir de la cola
        xQueueReceiveFromISR( selectedUart->onTxQueue,( void * ) &selectedUart->txBlock, &xTaskWokenByReceive );

        selectedUart->txLen = selectedUart->txBlock[0]+1; //en el primer byte del bloque viene la longitud del packete a enviar menos el CRC

        //	Calculo el CRC y lo pongo al final
        //selectedUart->txBlock[selectedUart->txLen + 1] = crc8_calc( 0,&selectedUart->txBlock[1], selectedUart->txLen );
        //selectedUart->txLen += 2; //Incremento en 2 txLen: uno por el byte extra al comienzon y otro para que el CRC se transmita
        selectedUart->txCounter++; //Incremento uno para no transmitir el primer byte del bloque

    }

    //	Si no llegué al final de la cadena transmito el dato
    if ( selectedUart->txCounter < selectedUart->txLen )
    {
        uartTxWrite( selectedUart->uartValue,selectedUart->txBlock[selectedUart->txCounter] );
        selectedUart->txCounter++;
    }

    //	Cuando termino de transmitir todos los bytes tengo que
    //	deshabilitar la interrupción e iniciar el timeout.
    if ( selectedUart->txCounter == selectedUart->txLen )
    {
        uartCallbackClr( selectedUart->uartValue, UART_TRANSMITER_FREE ); //Deshabilito la interrupcion de TX
        xTimerStartFromISR( selectedUart->onTxTimeOut, &xTaskWokenByReceive ); //Inicio el timer de Timeout
    }

    //Cierro la seccion critica
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

    //Hago la evaluacion de cambio de cambio de contexto necesario.
    if ( xTaskWokenByReceive != pdFALSE )
    {
        taskYIELD ();
    }
}
