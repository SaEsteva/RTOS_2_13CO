/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "checksum.h"
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

/* Recibe el mensaje y devuelve el checksum calculado*/
uint8_t str_to_checksum( char *mensaje )
{

	uint8_t i,checksum,sub_check,check_par,check_impar;

	uint8_t tamanio = mensaje[POS_LARGO];

    check_par = 0;
    check_impar = 0;

    for(i = POS_TAG_INICIAL+1 ; i < tamanio-1 ; i++)		// (LMxxxxxxxxxxxxC)
    {
    	if (i % 2 == 0)
		{
    		check_par = check_par + (mensaje[i]-PRIMER_DIGITO);
		}
    	else
    	{
    		check_impar = check_impar + (mensaje[i]-PRIMER_DIGITO);
    	}
    }
    sub_check = 3*check_par + check_impar;

    checksum = (10 - (sub_check % 10)) % 10;

    return checksum;
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/


