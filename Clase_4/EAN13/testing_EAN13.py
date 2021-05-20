import serial
import random
import time
import sys
import signal

"""
ANTES DE EJECUTAR ESTE SCRIPT
   Instalar pyserial. En la consola de anaconda (Anaconda prompt) ejectutar:
      conda install pyserial
  
EJECUTAR ESTE SCRIPT
   En la consola de anaconda (Anaconda prompt) ejectutar:
      cd path/to/script
      python data_gen_serial.py

-------------------------------------------------------------------------------

INFO de pyserial: http://pyserial.readthedocs.io/en/latest/shortintro.html

possible timeout values:
    1. None: wait forever, block call
    2. 0: non-blocking mode, return immediately
    3. x, x is bigger than 0, float allowed, timeout block call

 configure the serial connections (the parameters differs on the device you are
 connecting to)
 
ser = serial.Serial(
   port = "/dev/ttyUSB0",
   baudrate = 57600,
   parity = serial.PARITY_ODD,
   stopbits = serial.STOPBITS_TWO,
   bytesize = serial.SEVENBITS,
   timeout = 1
)
"""

# Variables globales con valores iniciales ------------------------------------


global EAN_12
global EAN_13
global EAN_13_tagged

global ser
ser = serial.Serial()

def signal_handler(sig, frame):
    print('Se pulsó Ctrl+C!, saliendo del programa!')
    try:
        ser.close()
    except:
        pass
    else:
        print('Se cerró el puerto OK.')
    sys.exit(0)    
    
# Funciones -------------------------------------------------------------------

def generar_EAN12():  

   global EAN_12
   EAN_12 = ""

   for i in range(12):
       EAN_12 = EAN_12 + str(random.randint(0,9))
   #print(str(len(EAN_12))+'\r\n')
   return

def agregar_checksum(valido = False):  

   global EAN_12
   global EAN_13
   
   if valido == False:
       EAN_13 = EAN_12 + 'F'
       return
   else:
       checksum = 0
       
       for i, digit in enumerate(EAN_12):
           checksum += int(digit) * 3 if (i % 2 == 0) else int(digit)
        
       agregar = str((10 - (checksum % 10)) % 10) 
        
       #print(':'+agregar+':'+'\r\n')
       EAN_13 = EAN_12 + agregar
       return
   
def agregar_tags(tag_inicial,tag_final):  

   global EAN_13
   global EAN_13_tagged
   
   EAN_13_tagged = tag_inicial+EAN_13+tag_final
   
   return


def sendData( tag_inicial,tag_final,valido = False ):
   #print( "Dato enviado --------------------------------------------\n" )
   
   
   generar_EAN12()
   agregar_checksum(valido)
   agregar_tags(tag_inicial,tag_final)

   print ("Enviando: ",  EAN_13_tagged)
   ser.write(str(EAN_13_tagged).encode())
   time.sleep(0.1)
   
   try:
      #print ("Leyendo ...")   
      readOut = ser.read(ser.in_waiting).decode('ascii')
      #time.sleep(1)
      print ("Recibido: ", readOut) 
   except:
      pass
   
   #print( "Dato enviado: " + EAN_13_tagged + '' )
   ser.flushInput()  # flush input buffer, discarding all its contents
   ser.flushOutput() # flush output buffer, aborting current output 
                            # and discard all that is in buffer
   
   if (readOut[0:-1] == EAN_13_tagged):
       estado = "Ok"
   else:
       estado = "Falla"
   
   print ("Estatus: "+estado)
   return


# comando help: Imprime la lista de comandos
def cmd_h():
   print( "Comandos disponibles -----------------------------------------------" )
   print( "   'h' (help) imprime esta lista de comandos." )
   print( "   'q' (quit) Salir del programa." )
   print( "   '1' Enviar EAN-13 tag inicial erroneo." )
   print( "   '2' Enviar EAN-13 tag final erroneo." )
   print( "   '3' Enviar EAN-13 con checksum mal." )
   print( "   '4' Enviar EAN-13 correcto." )
   print( "   '5' Enviar EAN-13 sin tag inicial." )
   print( "   'r' Enviar N EAN-13 validos." )
   print( "--------------------------------------------------------------------\n" )
   return

# comando 1:  Tag inicial erroneo
def cmd_1():
   sendData( '<',')',True )
   return

# comando 1:  Tag final erroneo
def cmd_2():
   sendData( '(','>',True )
   return

# comando 3: Checksum erroneo
def cmd_3():
   sendData( '(',')',False )
   return

# comando 4: Todo OK
def cmd_4():
   sendData( '(',')',True )   
   return

# comando 5: Tag inicial vacio
def cmd_5():
   sendData( '',')',True )   
   return

# comando r: Enviar un dato aleatorio del equipo con ID1.
def cmd_r():

   for i in range(20):
       sendData( '(',')',True )
   return            
 
# Inicializa y abre el puertos serie ------------------------------------------           
def uart_main():
    
    
    print( "\nconexión al puerto serie ----------------------------------------\n" )
    
    print("Ingrese el puerto serie, ejemplos: /dev/ttyUSB0 , COM1")
    print("O bien ingrese 'l' para /dev/ttyUSB0, o 'w' para COM4")
    
    receive = input()
    
    if receive == 'l':
       ser.port = "/dev/ttyUSB0"       # Puerto por defecto para Linux
    else:
       if receive == 'w':
          ser.port = "COM4"            # Puerto por defecto para Windows
       else:
          ser.port = receive
       
    ser.baudrate = 115200
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
    
          cmd_h()           # Imprime la lista de comandos
    
          # Ciclo infinito hasta comando exit (q) ---------------------------------
          while True: 
    
             command = ""
    
             # get keyboard input
             # input = raw_input(">> ")  # for Python 2
             command = input(">> ")      # for Python 3
    
             if command == 'q':
                print("Puerto cerrado. Se cierra el programa.")
                ser.close()
                exit()
    
             elif command == 'h':
                cmd_h()
    
             elif command == '1':
                cmd_1()
              
             elif command == '2':
                 cmd_2()
             
             elif command == '3':
                 cmd_3()    
    
             elif command == '4':
                 cmd_4()  

             elif command == '5':
                 cmd_5()  

             elif command == 'r':
                cmd_r()
                
             else:
                print("Comando no conocido.")
    
       except Exception as e1:
          print("error de comunicación." + str(e1))
    
    else:
       print("No se puede abrir el puerto serie.")
       exit()
       
#%%
signal.signal(signal.SIGINT, signal_handler)
       
def main():

    uart_main()
    
 
    
if __name__ == "__main__":
    main()       