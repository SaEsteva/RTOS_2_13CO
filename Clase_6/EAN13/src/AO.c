/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/
/*=====[Inclusion of own header]=============================================*/

#include "AO.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

/*===== Función activeObjectCreate()===========================================
 *
 * (+) Descripción: Esta función se encarga de crear el objeto activo; es decir,
 * crear su cola de procesamiento y su tarea asociada. Adicionalmente, se le
 * asignará una función de callback que es la que se ejecutará en la tarea.
 *
 * (+) Recibe: Un puntero del tipo "activeObject_t" al objeto activo y el
 * evento del tipo "activeObjectEvent_t".
 *
 * (+) Devuelve: True o False dependiendo de si el objeto activo se creó
 * correctamente o no.
 *
 *===========================================================================*/

bool_t activeObjectCreate( activeObject_t* ao, callBackActObj_t callback, TaskFunction_t taskForAO )
{
    // Una variable local para saber si hemos creado correctamente los objetos.
    BaseType_t retValue = pdFALSE;

    // Creamos la cola asociada a este objeto activo.
    ao->activeObjectQueue = xQueueCreate( N_QUEUE_AO, sizeof( activeObjectResponse_t ) );

    // Asignamos la tarea al objeto activo.
    ao->taskName = taskForAO;

    // Si la cola se creó sin inconvenientes.
    if( ao->activeObjectQueue != NULL )
    {
        // Asignamos el callback al objeto activo.
        ao->callbackFunc = callback;

        // Creamos la tarea asociada al objeto activo. A la tarea se le pasará el objeto activo como parámetro.
        retValue = xTaskCreate( ao->taskName, ( const char * )"Task For AO", configMINIMAL_STACK_SIZE*2, ao, tskIDLE_PRIORITY+2, NULL );
    }

    // Chequeamos si la tarea se creó correctamente o no.
    if( retValue == pdPASS )
    {
        // Cargamos en la variable de estado del objeto activo el valor "true" para indicar que se ha creado.
        ao->itIsAlive = TRUE;

        // Devolvemos "true" para saber que el objeto activo se instanció correctamente.
        return( TRUE );
    }
    else
    {
        // Caso contrario, devolvemos "false".
        return( FALSE );
    }
}

/*===========================================================================*/

/*===== Tarea activeObjectTask()===============================================
 *
 * (+) Descripción: Esta es la tarea asociada al objeto activo. Leerá datos de
 * la cola del objeto y cuando los procese, se ejecutará el callback asociado.
 *
 * (+) Recibe: Un puntero del tipo "void" por donde se enviará el puntero al
 * objeto activo.
 *
 * (+) Devuelve: Nada.
 *
 *===========================================================================*/

void activeObjectTask( void* pvParameters )
{
    // Una variable para evaluar la lectura de la cola.
    BaseType_t retQueueVal;

    // Una variable local para almacenar el dato desde la cola.
    activeObjectResponse_t  auxValue;

    // Obtenemos el puntero al objeto activo.
    activeObject_t* actObj = ( activeObject_t* ) pvParameters;

    // Cuando hay un evento, lo procesamos.
    while( TRUE )
    {
        // Verifico si hay elementos para procesar en la cola. Si los hay, los proceso.
        if( uxQueueMessagesWaiting( actObj->activeObjectQueue ) )
        {
            // Hago una lectura de la cola.
            retQueueVal = xQueueReceive( actObj->activeObjectQueue, &auxValue, portMAX_DELAY );

            // Si la lectura fue exitosa, proceso el dato.
            if( retQueueVal )
            {
                // Llamamos al callback correspondiente en base al comando que se le pasó.
                ( actObj->callbackFunc )( &auxValue );

                // TODO: ¡ATENCIÓN: BORRAR ESTA LÍNEA EN LA RELEASE FINAL!
                // Ponemos un delay de 500 ms para ver si está procesando los datos o no.
                //vTaskDelay(500/portTICK_RATE_MS);
            }
        }

        // Caso contrario, la cola está vacía, lo que significa que debo eliminar la tarea.
        else
        {
            // Cambiamos el estado de la variable de estado, para indicar que el objeto activo no existe más.
            actObj->itIsAlive = FALSE;

            // Borramos la cola del objeto activo.
            vQueueDelete( actObj->activeObjectQueue );

            // Y finalmente tenemos que eliminar la tarea asociada (suicidio).
            vTaskDelete( NULL );

        }
    }
}

/*===========================================================================*/

/*===== Función activeObjectEnqueue()==========================================
 *
 * (+) Descripción: Esta función se encargará de ingresar en la cola del objeto
 * activo un evento que deberá procesarse.
 *
 * (+) Recibe: Un puntero del tipo "activeObject_t" por donde se enviará el
 * puntero al objeto activo y un puntero a "char" donde se le pasará el dato a
 * encolar.
 *
 * (+) Devuelve: Nada.
 *
 *===========================================================================*/

void activeObjectEnqueue( activeObject_t* ao, activeObjectResponse_t* value )
{
    // Y lo enviamos a la cola.
    xQueueSend( ao->activeObjectQueue, value, 0 );
}

/*===========================================================================*/
