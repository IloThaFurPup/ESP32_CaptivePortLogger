import serial.tools.list_ports
import serial
ports = serial.tools.list_ports.comports()
portsList = []
import os
import os.path
import sys

use = None



usuario = os.getenv('USER', 'usuario_defecto')
inicio = os.path.join('/', 'home', usuario)
ruta_defecto_completa = os.path.join(inicio, 'Escritorio', 'datos.txt')

while True:    
    prompt = (
        f"Ingrese la ruta a escribir los datos: (0, por defecto {ruta_defecto_completa}) \n"
        f"Ruta base: {inicio}/"
    )
    
    rutainicialuser = input(prompt)
    
    # 2. PROCESAR LA ENTRADA
    if rutainicialuser in ('0', ''):
        ruta_final = ruta_defecto_completa
        
    elif rutainicialuser.startswith('/'):
        ruta_final = rutainicialuser
        
    else:
        ruta_final = os.path.join(inicio, rutainicialuser)

    print(f'\nLos datos se guardarán en: {ruta_final}, ¿Es correcto? (y/n)')
    
    respuesta = input().lower()

    if respuesta in ('y', ''):
        print('Okii, continuando... ')
        break
    
directorio_archivo = os.path.dirname(ruta_final)

Baudrate = input("Ingrese el Baudrate (0 = 9600 Arduino, 1 = 115200 ESP32): ")
if Baudrate == '0':
    Baudrate = 9600
elif Baudrate == '1':     
    Baudrate = 115200

for one in ports:
    portsList.append(str(one))
    print(str(one))

com = input("De donde viene el serial del arduino? ")

for i in range(len(portsList)):
    if portsList[i].startswith("/dev/ttyUSB" + str(com)):
        use = "/dev/ttyUSB" + str(com)
        print("Usando:", use)
        break

if use is None:
    print("Error: Puerto no encontrado.")
    exit(1)

arduino = serial.Serial(use, Baudrate, timeout=0.1)

print("Conectado. Esperando datos del modulo...")
print("Conectado. Esperando datos del modulo...")
while True:
    if arduino.in_waiting > 0:
        linea = arduino.readline().decode('utf-8').strip()
        print(f"Recibido: {linea}")

        if directorio_archivo:
            try:
                os.makedirs(directorio_archivo, exist_ok=True)
            except Exception as e:
                print(f"ERROR CRÍTICO: No se pudo crear el directorio '{directorio_archivo}': {e}")
                exit(1) 

        try:
            with open(ruta_final, 'a') as archivo:
                archivo.write(linea + '\n') 
        except IOError as e:
            print(f"Fallo al escribir en el archivo '{ruta_final}': {e}")
            exit(1)