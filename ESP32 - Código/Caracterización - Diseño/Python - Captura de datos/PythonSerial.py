import serial
import time

esp32Port = serial.Serial('COM4',115200)
time.sleep(2)

archivo = open("datos.txt", "w")


esp32Port.write(b'1')

while True:
    
    while(esp32Port.inWaiting):
        datoString = esp32Port.readline()
        datos = datoString.decode(errors='ignore')
        if datos[0] == "0":
            print("CAPTURANDO")
            archivo.write(datos)
        elif datos[0] == "1":
            print("TERMINADO")
            archivo.close()
            esp32Port.close()
            exit()
        else:
            pass



        

