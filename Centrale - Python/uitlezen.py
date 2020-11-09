import serial
import re

ser = serial.Serial('COM3', baudrate=9600)

def convertToC(value):
    return ((value * 5/1024)-0.5)*100

def convertToVoltage(value):
    return (value /100 + 0.5) * 1024 / 5

def convertToPercentage(value):
    return value / 1.024


while True:
    line = ser.readline()

    string = "".join(map(chr, line)).rstrip("\n")
    filteredString = re.sub('[^a-zA-Z0-9,]+', '', string) #haal de rommel uit de string
    print(filteredString)
    vals = filteredString.split(',')
    if vals[0] == "info": #info broadcast
        print("Info van de arduino:")
        print("type: "+vals[1])
        if vals[1] == "tmp":
            minTemp = convertToC(int(vals[2]))
            maxTemp = convertToC(int(vals[3]))
            print('min temp van zonnescherm (wanneer hij omhoog moet gaan): {}, in graden C: {}'.format(vals[2], minTemp))
            print('max waarde van zonnescherm (wanneer hij omlaag moet gaan) : {}, in graden C: {}'.format(vals[3], maxTemp))
        elif vals[1] == "light":
            minLight = convertToPercentage(int(vals[2]))
            maxLight = convertToPercentage(int(vals[3]))
            print('min waarde van zonnescherm: {}, {}%'.format(vals[2], minLight))
            print('max waarde van zonnescherm: {}, {}%'.format(vals[3], maxLight))
    else:
        if vals[0] == "tmp":
            print("het is een temperatuursensor")
            temp = convertToC(int(vals[1]))
            print('Het is {} graden'.format(temp))

        elif vals[0] == "light":
            print("het is een lichtsensor")
            percentage = convertToPercentage(int(vals[1]))
            print('Lichtsterkte is momenteel {}%'.format(percentage))
        if vals[2] == "up":
            print("de staat is omhoog")
            if vals[3] == "no":
                print("en hij gaat niet omhoog")
            else:
                print("en hij gaat omhoog")
        if vals[2] =="down":
            print("de staat is omlaag")
            if vals[3] == "no":
                print("en hij gaat niet omlaag")
            else:
                print("en hij gaat omlaag")

    print("--------")
