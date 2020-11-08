import serial
import struct
import binascii

#klasse om te gebruiken met een arduino naar keuze
class CentraleUnit: 
    def __init__(self, COMport, typemodel):
        self.COMport = COMport
        self.serialCon = serial.Serial(port=self.COMport, baudrate = 9600, timeout=2) #//Timeout op 2 laten staan, dit geeft de arduino genoeg denktijd om python the processen
        self.typemodel = typemodel

    def getCOMPort(self):
        return self.COMport
    
    def getSerialCon(self):
        return self.serialCon
    
    def getTypeModel(self):
        return self.typemodel

    def sendcommand(self, n): #// n = cijfer dat een actie triggert in de c code op de arduino
        self.serialCon.write(n.encode())

    def readCommand(self): #// readcommand is nodig om correct een byte te versturen. Zonder daanroep van deze methode werkt het versturen op serial niet. 
        self.serialCon.readline()

    


arduino = CentraleUnit("COM4", "TempSensor") #//Maakt een CentraleUnit object met een verbinding via COM4 met als typenaam TempSensor
arduino.readCommand() #//Readcommand is nodig om sendCommand correct te gebruiken
arduino.sendcommand('2') #//Verstuurt '2' als byte over de serial port naar de arduino