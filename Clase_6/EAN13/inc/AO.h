/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/
/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __AO_H__
#define __AO_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"

#define N_QUEUE_AO 		10
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*===== Tipo de dato activeObjectEvent_t ======================================
 *
 * (+) Descripción: Este tipo de dato se empleará para que el objeto activo
 * reaccione a cada uno de los tipos de eventos disponibles. Consiste en:
 *
 * - UPPER_CASE: Para indicar la función de mayusculizar.
 * - LOWER_CASE: Para indicar la función de minusculizar.
 *
 *===========================================================================*/

typedef struct
{
    char* string;
    QueueHandle_t colaCapa3;
} activeObjectResponse_t;


/*===== Tipo de dato callBackActObj_t =========================================
 *
 * (+) Descripción: Tipo de dato para funciones callback que se ejecutarán en
 * el objeto activo.
 *
 *===========================================================================*/

typedef void ( *callBackActObj_t )( activeObjectResponse_t* );


/*===== Objeto activeObjectEvent_t ============================================
 *
 * (+) Descripción: Este tipo de dato se utiliza para almacenar todos los
 * elementos asociados con el funcionamiento del objeto activo. Este tipo de
 * variable esta conformado por:
 *
 * - TaskFunction_t: Una variable para poder crear la tarea asociada al OA.
 * - QueueHandle_t: La cola del objeto activo en cuestión.
 * - callBackActObj_t: Callback que se deberá ejecutar en la tarea del OA.
 * - bool: Una variable de tipo booleana para saber si el objeto activo existe
 * o no.
 *
 *===========================================================================*/

typedef struct
{
    TaskFunction_t taskName;
    QueueHandle_t activeObjectQueue;
    callBackActObj_t callbackFunc;
    bool_t itIsAlive;
} activeObject_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

/*===== Función activeObjectCreate()===========================================
 *
 * (+) Descripción: Esta función se encarga de crear el objeto activo; es decir,
 * crear su cola de procesamiento y su tarea asociada. Adicionalmente, se le
 * asignará una función de callback que es la que se ejecutará en la tarea.
 *
 * (+) Recibe: Un puntero del tipo "activeObject_t" al objeto activo y el
 * evento del tipo "activeObjectEvent_t". Adicionalmente, se le debe pasar el
 * nombre de la tarea asociada al objeto activo que se va a crear, del tipo
 * "TaskFunction_t".
 *
 * (+) Devuelve: True o False dependiendo de si el objeto activo se creó
 * correctamente o no.
 *
 *===========================================================================*/

bool_t activeObjectCreate( activeObject_t* ao, callBackActObj_t callback, TaskFunction_t taskForAO );


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

void activeObjectTask( void* pvParameters );


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

void activeObjectEnqueue( activeObject_t* ao, activeObjectResponse_t* value );


/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __AO_H__ */
