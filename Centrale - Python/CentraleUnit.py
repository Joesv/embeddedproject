import serial
import time 

#klasse om te gebruiken met een arduino naar keuze
class CentraleUnit: 
    def __init__(self, COMport):
        self.COMport = COMport
        self.serialCon = serial.Serial(port=self.COMport, baudrate = 19200) #//Timeout op 2 laten staan, dit geeft de arduino genoeg denktijd om python the processen
        self.typemodel = ""
        self.minvalue = 0
        self.maxvalue = 0

    def getCOMPort(self):
        return self.COMport
    
    def getSerialCon(self):
        return self.serialCon
    
    def getTypeModel(self):
        return self.typemodel

    def getMinValue(self):
        return self.minvalue

    def getMaxValue(self):
        return self.maxvalue

    def getInitData(self):
        initdata = self.serialCon.readline().decode() #Ontcijfer wat er binnenkomt
        typemodel = initdata[5:8] #Wat voor type arduino het betreft, word doorgestuurd vanuit C

        if typemodel == 'tmp': #Als het een temperatuursensor betreft:
            self.typemodel = "TempSensor"
            self.minvalue = initdata[10:13]
            self.maxvalue = initdata[14:]
        else: #Als het een lichtsensor betreft:
            typemodel = "LightSensor"
    
    def readCommand(self): #// readcommand is nodig om correct een byte te versturen. Zonder aanroep van deze methode werkt het versturen op serial niet. 
        self.serialCon.readline()

    def sendcommand(self, n): #// n = cijfer dat een actie triggert in de c code op de arduino
        self.readCommand() #//Readcommand is nodig om sendCommand() correct te gebruiken
        self.serialCon.write(n.encode())

    def receiveData(self): #Methode om data op te halen vanuit de seriële verbinding met de arduino
        data = self.serialCon.readline().decode("ascii")
        return data

    def checkPort(self): #//Returnt welke poort er daadwerkelijk wordt gebruikt voor seriële communicatie
        return self.serialCon.name

    

########## TEST CODE ###########

arduino = CentraleUnit("COM4") #//Maakt een CentraleUnit object met een verbinding via COM4 met als typenaam TempSensor
arduino.getInitData()
print(arduino.getTypeModel())
print(arduino.getMinValue())
print(arduino.getMaxValue())
arduino.sendcommand('1') #//Verstuurt '2' als byte over de serial port naar de arduino 
print(arduino.checkPort())
print(arduino.receiveData())
time.sleep(2)
print(arduino.receiveData())