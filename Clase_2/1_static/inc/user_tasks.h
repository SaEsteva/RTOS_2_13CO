/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

#ifndef USER_TASKS_H_
#define USER_TASKS_H_
/*==================[declaraciones de funciones internas]====================*/
void gpio_init(void);
/*==================[declaraciones de tareas]====================*/
void tarea_A( void* taskParmPtr );
void tarea_C( void* taskParmPtr );

#endif /* USER_TASKS_H_ */
