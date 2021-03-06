import serial #benodigde import voor seriële verbinding
import time
import re #regex om te filteren

#klasse om te gebruiken met een arduino naar keuze
class CentraleUnit: 
    def __init__(self, COMport):
        self.COMport = COMport
        self.serialCon = serial.Serial(port=self.COMport, baudrate = 19200)
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
    
    def setTemperature(self, temperature):
        self.temperature = temperature
    
    def getTemperature(self):
        return self.temperature

    def convertToC(self, value): #Formule om waarde van C om te rekenen naar graden Celcius
        return ((value * 5/1024)-0.5) *100
    
    def convertToVoltage(self, value): #Formule om graden Celsius om te rekenen naar Voltage
        return (value / 100 + 0.5) * 1024 / 5

    def getInitData(self):
        initdata = self.serialCon.readline().decode() #Ontcijfer wat er binnenkomt
        filteredInit = self.filterString(initdata) #eventuele onzin eruit filteren
        splitted = filteredInit.split(",") #in een lijst splitten op basis van de comma
        typemodel = splitted[1]
        if typemodel == 'tmp': #Als het een temperatuursensor betreft:
            self.typemodel = "TempSensor"
            self.minvalue = int(splitted[2])
            self.maxvalue = int(splitted[3])
            self.minvalue = self.convertToC(self.minvalue)
            self.maxvalue = self.convertToC(self.maxvalue)
        else: #Als het een lichtsensor betreft:
            self.typemodel = "LightSensor"
            self.minvalue = splitted[2]
            self.maxvalue = splitted[3]
    
    def readCommand(self): #// readcommand is nodig om correct een byte te versturen. Zonder aanroep van deze methode werkt het versturen op serial niet. 
        self.serialCon.readline().decode("ascii")

    def sendcommand(self, n): #// n = cijfer dat een actie triggert in de c code op de arduino
        self.readCommand() #//Readcommand is nodig om sendCommand() correct te gebruiken
        self.serialCon.write(n.encode())

    def sendMinSetting(self, n):
        if isinstance(n, int): # Als n een cijfer is dan is het een temperatuurinstelling die verstuurt moet worden
            if n <= 255: #Getal moet in 8 bits passen
                n = str(n)
                self.readCommand() #//Readcommand is nodig om sendCommand() correct te gebruiken
                self.serialCon.write(n.encode())
            else:
                pass

    def sendMaxSetting(self, n):
        if isinstance(n, int): # Als n een cijfer is dan is het een temperatuurinstelling die verstuurt moet worden
            if n <= 255: #Getal moet in 8 bits passen
                n = str(n)
                self.readCommand() #//Readcommand is nodig om sendCommand() correct te gebruiken
                self.serialCon.write(n.encode())
            else:
                pass

    def receiveData(self): #Methode om data op te halen vanuit de seriële verbinding met de arduino
        data = self.serialCon.readline().decode("ascii")
        return data

    def checkPort(self): #//Returnt welke poort er daadwerkelijk wordt gebruikt voor seriële communicatie
        return self.serialCon.name
    def filterString(self, data):
        return re.sub('[^a-zA-Z0-9,]+', '', data)
    

########## TEST CODE ###########

#arduino = CentraleUnit("COM4") #//Maakt een CentraleUnit object met een verbinding via COM4 met als typenaam TempSensor
#arduino.getInitData()
#print(arduino.getTypeModel())
#print(arduino.getMinValue())
#print(arduino.getMaxValue())
#arduino.sendcommand('1') #//Verstuurt '2' als byte over de serial port naar de arduino 
#print(arduino.checkPort())
#print(arduino.receiveData())
#time.sleep(2)
#print(arduino.receiveData())