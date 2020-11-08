import serial

ser = serial.Serial('COM3', baudrate=19200)
type = 0
while True:
    '''bytes = ser.read(2)
    print(bytes.hex())
    res = int.from_bytes(bytes, "big") #terug naar een schaal van 0 t/m 1023 ipv 0 t/m 255
    res *= 5
    res /= 1024
    degrees = (res - 0.5) * 100
    #print("degrees C: " + str(degrees))
    ##print(int.from_bytes(bytes, "big"))'''
    line = ser.readline()

    string = "".join(map(chr, line)).rstrip("\n")
    print(string)
    '''
    details
    if string == "tmp":
        type = 2

    elif string == "light":
        type = 3

    else:
        if type == 2:
            value = 5 * int(string)
            value /= 1024
            degreesC = (value - 0.5) * 100
            print("degrees C: " + str(degreesC))
        if type == 3:
            percentage = int(string) / 1024 * 100
            print("percentage: " + str(percentage)) '''
