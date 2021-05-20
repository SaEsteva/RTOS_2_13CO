import serial
import random
import time
import sys
import signal

from crc8 import *

#agrega un crc. Si valido es False, entonces el CRC es invalido
def c2_add_crc(payload, valido = True):
    hash = crc8()
    hash.update(bytes(payload,'utf-8'))
    crc = hash.hexdigest()
    #print(type(crc))
    if valido:
        payload += str( crc ).upper()
    else:
        payload += str( "aa").upper()
    return payload


def c2_add_delimiters(payload,start_of_msg , end_of_msg):

   payload = start_of_msg + payload + end_of_msg

   return payload

#toma una lista de payloads y le formate ael frame y devuelve lista
def c2_create_frame(  payload , start_of_msg , end_of_msg ,valido = False ):
    cmd = c2_add_crc( payload , valido)
    cmd = c2_add_delimiters(cmd, start_of_msg ,end_of_msg)
    return cmd

'''
devuelve el overhead agregado por C2
2 de separadore4s mas 2 de crc
'''
def c2_overhead():
    return 4