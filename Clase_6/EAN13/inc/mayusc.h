/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

#ifndef __MAYUS_MINUS_H__
#define __MAYUS_MINUS_H__

#include "AO.h"

#define TAG_INICIAL '('
#define TAG_FINAL ')'
#define ERROR_TAG_INICIAL " Tag inicial incorrecto"
#define ERROR_TAG_FINAL " Tag final incorrecto"
#define ERROR_NUMERICO " Todos los datos deben ser NUMEROS"

#define PRIMER_DIGITO '0'
#define ULTIMO_DIGITO '9'

#define POS_LARGO 0
#define POS_TAG_INICIAL 1
#define POS_COMANDO 2

void Check_packet( activeObjectResponse_t* data );
void Sietizador( activeObjectResponse_t* data );

void capitalize( activeObjectResponse_t* data );
void lowercase( activeObjectResponse_t* data );
char getCommand( char* string );

#endif
