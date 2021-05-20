#!/usr/bin/python
# -*- coding: UTF-8 -*-

import serial
import random
import time
import sys
import signal
import crc8
import random
import string

from os import system
from serial_init import *
from c2 import *
from c3 import *

"""
------------------------------------------------------------------------------
requirements:
this script needs pyserial
refer to https://pyserial.readthedocs.io/en/latest/pyserial.html for installing instructions
this script must use python3
------------------------------------------------------------------------------
script instructions:

execute:
python test_random.py
------------------------------------------------------------------------------
"""

# config -------------------------------------------------------------------
DEFAULT_BAURDATE = 115200
SOM = '('
EOM = ')'
T_INTER_FRAME = 0.06 #SEGUNDOS -> 60ms
DEFAULT_CMD_NRO = 10
DEFAULT_BYTE_MAX_COUNT = 200
DEFAULT_BYTE_MIN_COUNT = 10
DEFAULT_MODE = 1
T_INTER_FRAME_LARGUISIMO = 3
T_FRAME_POLL = T_INTER_FRAME

#EN 1 SE AGREGA LA OPERACION EN LA RTA ESPERADA EN 0 NO
RESPONSE_HAS_OPERATION = 1

def clear_screen():
    if sys.platform.startswith('win'):
        _ = system('cls')
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        _ = system('clear')
    #elif sys.platform.startswith('darwin'):
        # como se hace?
    else:
        raise EnvironmentError('Unsupported platform')

#SIGNAL TRAP
def signal_handler(sig, frame):
    print('Se pulso Ctrl+C!, saliendo del programa!')
    try:
        ser.close()
    except:
        pass
    else:
        print('Se cerro el puerto OK.')
    sys.exit(0)

# Funciones -------------------------------------------------------------------

#SENDS A LIST OF COMMANDS
def send_frames( ser, frames , delay_between ):
    lista = []
    lista_tiempos = []

    for i in range(0,len(frames)):

        lista_tiempos.append(time.perf_counter())

        ser.write( str(frames[i]).encode() )

        '''
        Espero un delay para comenzar a recibir
        Durante este tiempo, la trama debe procesarse en la EDUCIAA, y enviarse una respuesta.
        '''
        time.sleep(delay_between)

        try:
            #leo los datos que YA ESTAN DISPONIBLES para leer
            readOut = ser.read(ser.in_waiting).decode('ascii')
            lista.append(readOut)
        except:
            pass

    return lista , lista_tiempos

def send_frames_2( ser , frames , total_timeout ):
    lista = []
    lista_tiempos = []

    #for i in range(0,len(frames)):
    for frame in frames:
        tiempo = time.perf_counter()
        ser.write( str(frame).encode() )

        readOut=""

        try:
            total = 0

            while(True):
                time.sleep( T_FRAME_POLL )

                if(readOut != ""  ):
                    break

                #print(str(total) + " " + str(total_timeout) )

                if(total_timeout<total):
                    break

                readOut = ser.read(ser.in_waiting).decode('ascii')
                total += T_FRAME_POLL

                if(readOut != ""  ):
                    #registro tiempo, si llego algo
                    lista_tiempos.append(tiempo)
                    lista.append(readOut)

        except:
            pass

    return lista , lista_tiempos




def randomword(length):
   letters =  string.ascii_letters
   return ''.join(random.choice(letters) for i in range(length))

def generar_cadenas(N,MAX):
    lista = [  ]
    for i in range(0,N):
        sim_chars = random.randint(1,MAX)
        word = randomword(sim_chars)
        #print(word)
        #print(type(randomword(sim_chars)))
        lista.append(word)

    #print(lista)
    return lista

#genera N operaciones del dictionario, devuelve lista
def generar_operaciones(N,dict):
    lista = []
    #print(len(dict))
    for i in range(0,N):
        index = random.randint(0,len(dict)-1)
        lista.append(dict[index])
    return lista


def validar( cmd_raw , rta , op ):
    #print(type(string.ascii_letters + '(' + ')'))
    #limpio non printable
    #print(bytes(rta,'utf-8'))
    #rta = ''.join(c for c in rta if (c in (string.ascii_letters+ '(' + ')') )
    rta = rta.rstrip('\n')
    rta = rta.rstrip('\r')
    rta = rta.lstrip('\n')
    rta = rta.lstrip('\r')
    rta = rta.strip()

    #proceso
    cmd_raw = c3_process( cmd_raw , op )

    if(RESPONSE_HAS_OPERATION==0):
        op= ""

    cmd = create_pkt( op , cmd_raw , SOM , EOM , True )

    if( cmd != rta ):
        print( "                esperado: " + cmd)
        print( "                recibido: " + rta)
        return 0
    else:
        return 1

def input_number(msg,default):
    tmp_ = input( msg + " ( default: "+ str(default) + " ):" )
    if(tmp_==''):
        nro = default
    else:
        nro = int(tmp_ )
    return nro

def input_char(msg , default):
    tmp_ = input( msg + " ( default: "+ str(default) + " ):" )
    if(tmp_==''):
        caracter = default
    else:
        caracter = tmp_
    return caracter

#crea un paquete completo
def create_pkt( operacion , payload , som , eom , valido = False ):
    temp = c3_add_op( payload , operacion )
    pkt =  c2_create_frame( temp , som , eom , valido )
    return pkt

#toma una lista de payloads y le formatea el frame y devuelve lista
def create_pkts( operaciones , payloads , som , eom , valido = False ):
    lista = []
    if(len(operaciones)!=len(payloads) ):
        print("Error")
        return

    for i in range(0,len(operaciones)):
        cmd = create_pkt( operaciones[i] ,  payloads[i] , som , eom , valido )
        lista.append(cmd)
    return lista

def modo_random(serial):
    cantidad_comandos 	= input_number("Ingrese cantidad de comandos",DEFAULT_CMD_NRO)
    cantidad_bytes 		= input_number("Ingrese cantidad de bytes maxima por paquete",DEFAULT_BYTE_MAX_COUNT)
    #DEFAULT_BYTE_MIN_COUNT = 10 TODAVIA NO UTILIZADO

    operaciones = generar_operaciones( cantidad_comandos , C3_OPERATIONS )

    respuestas = []
    cadenas = generar_cadenas(cantidad_comandos , cantidad_bytes - c2_overhead() - c3_overhead() )

    #genero los frames de todos los comandos
    frames = create_pkts( operaciones , cadenas , SOM , EOM , True )

    #envio paquetes
    respuestas , tiempostx = send_frames( serial , frames , T_INTER_FRAME )

    tiempostx = [0.0] + tiempostx
    #print(type(tiempos))

    #validacion
    error_count = 0
    for i in range(0,len(cadenas)):
        print("@" + "{:.4f}".format(tiempostx[i+1]) + " dt: " + "{:.3f}".format(tiempostx[i+1]-tiempostx[i]) + " ==================================================================================================================================  " )
        print("cmd: " + frames[i] )
        print("rta: " + respuestas[i])
        if( validar( cadenas[i] , respuestas[i] , operaciones[i] )==0 ):
            error_count+=1
        print("")

    if(error_count>0):
        print("errores:" + str(error_count))
        print("")

    return

def modo_unitario(serial):
    opt = "1"

    while( opt != "q" ):

        lista=[]
        #clear_screen()
        print("q- salir")
        print("1- Trama con CRC erroneo")
        print("2- Trama con caracteres erroneos")
        print("3- Trama operacion erronea")
        print("4- Trama sin cerrar")
        print("5- Trama con tamaño mayor al buffer de " + str(DEFAULT_BYTE_MAX_COUNT) )
        print("6- Trama sin cerrar junto a un paquete bueno" )
        opt = input_char("Ingrese opcion", 'q')

        if( opt=="1"):
            cmd = create_pkt( 'm' , "HolaMundo" , SOM , EOM  )
            lista.append(cmd)
            respuestas , tiempos = send_frames_2( serial , lista , T_INTER_FRAME_LARGUISIMO )
            #print( str(respuestas) + " " + str(len(respuestas)))
            if(len(respuestas)==0):
                print("PASS")
            else:
                print("FAIL")

        if( opt=="2"):
            expected = "(ERROR165)"
            cmd = create_pkt( 'm' , "HolaMundo!" , SOM , EOM , True )
            print(cmd)
            lista.append(cmd)
            respuestas , tiempos = send_frames_2( serial , lista , T_INTER_FRAME )
            #print(respuestas)
            if(len(respuestas)==1 and respuestas[0]==expected):
                print("PASS")
            else:
                print("FAIL" + " received:" + respuestas[0] + " expected: "+  expected)

        if( opt=="3"):
            expected = "(ERROR26C)"
            cmd = create_pkt( 'X' , "HolaMundo" , SOM , EOM , True )
            lista.append(cmd)
            respuestas , tiempos = send_frames_2( serial , lista , T_INTER_FRAME )
            #print(respuestas)
            if(len(respuestas)==1 and respuestas[0]==expected):
                print("PASS")
            else:
                print("FAIL" + " received:" + respuestas[0] + " expected: "+  expected)

        if( opt=="4"):
            cmd = create_pkt( 'm' , "HolaMundo" , SOM , EOM , True )
            cmd = cmd[:-1] #quito un caracter
            #print(cmd)
            lista.append(cmd)
            respuestas , tiempos = send_frames_2( serial , lista , T_INTER_FRAME_LARGUISIMO )
            #print( str(respuestas) + " " + str(len(respuestas)))
            if(len(respuestas)==0):
                print("PASS")
            else:
                print("FAIL")

        if( opt=="5"):
            payload = randomword(DEFAULT_BYTE_MAX_COUNT+100)
            cmd = create_pkt( 'm' , payload , SOM , EOM , True )
            print(cmd)
            lista.append(cmd)
            respuestas , tiempos = send_frames_2( serial , lista , T_INTER_FRAME_LARGUISIMO )
            #print( str(respuestas) + " " + str(len(respuestas)))
            if(len(respuestas)==0):
                print("PASS")
            else:
                print("FAIL")

        if( opt=="6"):
            expected = "(MMUNDO75)"
            cmd = create_pkt( 'm' , "hola" , SOM , EOM , True )
            cmd = cmd[:-2] + create_pkt( 'M' , "mundo" , SOM , EOM , True ) #quito dos caracter + otro paquete
            print(cmd)
            lista.append(cmd)
            respuestas , tiempos = send_frames_2( serial , lista , T_INTER_FRAME_LARGUISIMO )
            #print( str(respuestas) + " " + str(len(respuestas)))
            if(len(respuestas)==1 and respuestas[0]==expected):
                print("PASS")
            else:
                print("FAIL" + " received:" + respuestas[0] + " expected: "+  expected)

    return

# Inicializa y abre el puertos serie ------------------------------------------
def uart_main():

    #platform
    print("Plataforma: " + sys.platform )

    #seleccion de puerto serie
    port = serial_choose()

    #declaro el objeto de puerto serie (sin abrirlo)
    ser = serial.Serial()

    #parametros del puerto serie

    ser.port = port
    ser.baudrate = DEFAULT_BAURDATE
    ser.bytesize = serial.EIGHTBITS    # number of bits per bytes # SEVENBITS
    ser.parity = serial.PARITY_NONE    # set parity check: no parity # PARITY_ODD
    ser.stopbits = serial.STOPBITS_ONE # number of stop bits # STOPBITS_TWO
    #ser.timeout = None                 # block read
    ser.timeout = 1                    # non-block read
    #ser.timeout = 2                    # timeout block read
    ser.xonxoff = False                # disable software flow control
    ser.rtscts = False                 # disable hardware (RTS/CTS) flow control
    ser.dsrdtr = False                 # disable hardware (DSR/DTR) flow control
    ser.writeTimeout = 2               # timeout for write

    try:
       ser.open()
    except Exception as e:
       print("Error abriendo puerto serie.\n" + str(e) + '\nFin de programa.')
       exit()

    # Si pudo abrir el puerto -----------------------------------------------------

    if ser.isOpen():

        print(ser.name + ' abierto.\n')

        try:
            ser.flushInput()  # flush input buffer, discarding all its contents
            ser.flushOutput() # flush output buffer, aborting current output
                              # and discard all that is in buffer

            print("1- Modo random")
            print("2- Modo test unitario")
            modo = input_number("Ingrese tipo test: ",DEFAULT_MODE)

            if(modo == 1):
                modo_random(ser)
            elif( modo== 2 ):
                modo_unitario(ser)
            else:
                print("no se eligio modo, ta luego")

            print("Puerto cerrado. Se cierra el programa.")
            ser.close()
            exit()

        except Exception as e1:
            print(e1)
            #print("error de comunicacion." + str(e1))

    else:
       print("No se puede abrir el puerto serie.")
       exit()

#%%
signal.signal(signal.SIGINT, signal_handler)

def main():

   # cmd_1()
   uart_main()

if __name__ == "__main__":
    main()

