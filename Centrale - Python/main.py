import serial

ser = serial.Serial('COM3', baudrate = 9600, timeout=1) #Start een verbinding op COM3 met een baudrate van 9600 

def getValues(): #Request, read and return data from Arduino
     ser.write(b'g') #Verzend een byte naar serial bus, en stuur letter 'g'
     arduinoData = ser.readline().decode('ascii') #lees lijn die binnenkomt vanaf arduino en decode deze naar ascii
     return arduinoData 

while(1):

     userInput = input('Get data point?')

     if userInput == 'y':
          print(getValues())