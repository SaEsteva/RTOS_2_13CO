/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/
#include "mayusc.h"

#include <string.h>

void Check_packet( activeObjectResponse_t* data )
{
    int i=2;//indice arranca en uno porque el primer byte es el tamaño del string

    int largo = data->string[POS_LARGO];

    if (data->string[POS_TAG_INICIAL] != TAG_INICIAL)
    {
    	strcpy( data->string, ERROR_TAG_INICIAL );
    	data->string[POS_LARGO]=sizeof(ERROR_TAG_INICIAL); //Indico cuantos bytes tiene el string
		return;
    }

    if (data->string[largo] != TAG_FINAL)
        {
        	strcpy( data->string, ERROR_TAG_FINAL );
        	data->string[POS_LARGO]=sizeof(ERROR_TAG_FINAL); //Indico cuantos bytes tiene el string
    		return;
        }

    while ( largo > i+1 )
    {

        if ( ( data->string[i]>=PRIMER_DIGITO )&&( data->string[i]<=ULTIMO_DIGITO) ) //si el caracter esta entre a y z los paso a mayuscula
        {
            //string[i]=string[i]+( 'A'-'a' );
        }
        else
        {
            strcpy( data->string, ERROR_NUMERICO );
            data->string[POS_LARGO]=sizeof(ERROR_NUMERICO); //Indico cuantos bytes tiene el string
            return;
        }
        i++;
    }
    xQueueSend( data->colaCapa3, data, 0 );
}

void Sietizador( activeObjectResponse_t* data )
{
	int i=2;//indice arranca en uno porque el primer byte es el tamaño del string

	    int largo = data->string[POS_LARGO];

	    if (data->string[POS_TAG_INICIAL] != TAG_INICIAL)
	    {
	    	strcpy( data->string, ERROR_TAG_INICIAL );
	    	data->string[POS_LARGO]=sizeof(ERROR_TAG_INICIAL); //Indico cuantos bytes tiene el string
			return;
	    }

	    if (data->string[largo] != TAG_FINAL)
	        {
	        	strcpy( data->string, ERROR_TAG_FINAL );
	        	data->string[POS_LARGO]=sizeof(ERROR_TAG_FINAL); //Indico cuantos bytes tiene el string
	    		return;
	        }

	    while ( largo > i+1 )
	    {

	        if ( ( data->string[i]>=PRIMER_DIGITO)&&( data->string[i]<=ULTIMO_DIGITO ) ) //si el caracter esta entre a y z los paso a mayuscula
	        {
	            data->string[i]=7+PRIMER_DIGITO;
	        }
	        else
	        {
	            strcpy( data->string, ERROR_NUMERICO );
	            data->string[POS_LARGO]=sizeof(ERROR_NUMERICO); //Indico cuantos bytes tiene el string
	            return;
	        }
	        i++;
	    }
	    xQueueSend( data->colaCapa3, data, 0 );
}

char getCommand( char* string )
{
    // Lo �nico que hacemos es devolver el comando que se encuentra
    // en el paquete recibido.
    return( string[POS_COMANDO] );
}

