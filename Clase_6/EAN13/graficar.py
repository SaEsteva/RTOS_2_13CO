import serial
import random
import time
import sys
import signal

import datetime as dt
import matplotlib.pyplot as plt
import matplotlib.animation as animation


# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []
ys = []
N = 20

# Variables globales con valores iniciales ------------------------------------


global EAN_12
global EAN_13
global EAN_13_tagged


global ser
ser = serial.Serial()

def animate(i, xs, ys):

   dato = comando() # Escritura y lectura

   try:
      dato_split = [int(dato[1:5]),int(dato[5:9]),int(dato[9:13])]
      print("cosa: ", dato_split)
    
      # Add x and y to lists
      
      if dato_split[0] != '':
         xs.append(dt.datetime.now().strftime('%H:%M:%S.%f')) 
         ys.append(dato_split)

         # Limit x and y lists to 20 items
         xs = xs[-N:]
         ys = ys[-N:]

         # Draw x and y lists
         ax.clear()
         ax.plot(xs, ys)

         # Format plot
         plt.xticks(rotation=45, ha='right')
         plt.subplots_adjust(bottom=0.30)
         plt.title('EAN-13')
         plt.ylabel('Digitos de EAN-13')
   except:
      pass


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
   
   return readOut


# comando : Todo OK
def comando():
   dato = sendData( '(',')',True )   
   return dato
 
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
    
          while True:      
             
            # Leo por serial 

            #( AAAA BBBB CCCC X)

            # [ int(AAAA) int(BBBB) int(CCCC) ]


            # Set up plot to call animate() function periodically
            ani = animation.FuncAnimation(fig, animate, fargs=(xs, ys), interval=3000)
            plt.show()

            #time.sleep(10)
            
                         

    
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

