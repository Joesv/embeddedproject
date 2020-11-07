import serial
import struct
import binascii

#klasse om te gebruiken met een arduino naar keuze
class CentraleUnit: 
    def __init__(self, COMport, typemodel):
        self.COMport = COMport
        self.serialCon = serial.Serial(port=self.COMport, baudrate = 9600)
        self.typemodel = typemodel

    def getCOMPort(self):
        return self.COMport
    
    def getSerialCon(self):
        return self.serialCon
    
    def getTypeModel(self):
        return self.typemodel

    def sendcommand(self):
        self.serialCon.write(bytes([1]))

arduino = CentraleUnit("COM4", "jaweekveel")
arduino.sendcommand()