import serial

ser = serial.Serial('COM4', baudrate = 9600) #Start een verbinding op COM3 met een baudrate van 9600 

def getValues(): #Request, read and return data from Arduino
     ser.write(b'g') #Verzend een byte naar serial bus, en stuur letter 'g'
     arduinoData = ser.readline().decode('ascii') #lees lijn die binnenkomt vanaf arduino en decode deze naar ascii
     return arduinoData 

def TurnOnOne():
     ser.write(bytes([2]))


TurnOnOne()
ser.close()