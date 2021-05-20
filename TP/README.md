# SCRIPT

Este script se desarrolló para probar la performance de los trabajos prácticos desarrollados por alumnos de la asignatura **Sistemas Operativos en Tiempo Real 2** de la **Carrera de Especialización en sistemas Embebidos** de FiUBA.

# Requisitos

* pyserial
* python3
* OS: windows o linux.

# Línea de comandos

```python3  test_random.py```

(en linux, posiblemente se necesite llamar con sudo)

# Configuraciones dentro de test_random.py

* DEFAULT_BAURDATE : baudrate para el puerto serie
* SOM : start of message.
* EOM : end of message.
* T_INTER_FRAME : tiempo en sengundos que esperará para enviar comandos
* DEFAULT_CMD_NRO : nro por omisión de la cantidad de comandos a generar
* DEFAULT_BYTE_MAX_COUNT : nro por omisión de la cantidad máxima de bytes por paquete.
* DEFAULT_BYTE_MIN_COUNT : nro por omisión de la cantidad mínima de bytes por paquete (no se usa todavía)

# Colaboradores

* Diego Essaya y Santiago Abbate (versión inicial)

