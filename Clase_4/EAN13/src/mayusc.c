/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/
#include "mayusc.h"

#include <string.h>
void Check_packet( char* string )
{
    int i=2;//indice arranca en uno porque el primer byte es el tamaño del string

    // string = (M2342342342440)
    // string = "ERRRO_("
    // option = string[2] = M

    int largo = string[POS_LARGO];

    if (string[POS_TAG_INICIAL] != TAG_INICIAL)
    {
    	strcpy( string, ERROR_TAG_INICIAL );
		string[POS_LARGO]=sizeof(ERROR_TAG_INICIAL); //Indico cuantos bytes tiene el string
		return;
    }

    if (string[largo] != TAG_FINAL)
        {
        	strcpy( string, ERROR_TAG_FINAL );
    		string[POS_LARGO]=sizeof(ERROR_TAG_FINAL); //Indico cuantos bytes tiene el string
    		return;
        }

    while ( largo > i+1 )
    {

        if ( ( string[i]>=PRIMER_DIGITO )&&( string[i]<=ULTIMO_DIGITO ) ) //si el caracter esta entre a y z los paso a mayuscula
        {
            //string[i]=string[i]+( 'A'-'a' );
        }
        else
        {
            strcpy( string, ERROR_NUMERICO );
            string[POS_LARGO]=sizeof(ERROR_NUMERICO); //Indico cuantos bytes tiene el string
            return;
        }
        i++;
    }
}



