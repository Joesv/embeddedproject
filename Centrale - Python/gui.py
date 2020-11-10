import tkinter as tk
from tkinter import *
import sys
sys.path.append("..") # Path zetten voor imports
from CentraleUnit import CentraleUnit # Import van CentraleUnit klasse
import time
import serial
from serial import SerialException

portnames = {"COM0", "COM1", "COM2", "COM3", "COM4", "COM5"}

class MainUI(tk.Tk):
    # Er moet hier ergens een while loop komen die om de halve minuut gegevens van de arduino ophaalt

    def __init__(self, *args, **kwargs):
        
        self.arduinolist = [] #Lijst waar de arduino's in komen te staan
        self.isRunning = True # Boolean om aan te geven of het programma nog draait
        self.sluitUnitAan()
                   
        tk.Tk.__init__(self, *args, **kwargs)
        
        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)
        
        self.frames = {}

        for F in (Unit1, Unit2):

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(Unit1)

    def show_frame(self, cont):

        frame = self.frames[cont]
        frame.tkraise()

    def portIsUsable(self, portName): ## Kijkt of de poorten bruikbaar zijn
        try:
            ser = serial.Serial(port=portName)
            return True
        except:
            return False
    
    def sluitUnitAan(self): #Methode om Arduino aan te sluiten.
            for i in portnames: 
                if self.portIsUsable(i) == True:
                    arduino = CentraleUnit(i)
                    arduino.getInitData()
                    self.arduinolist.append(arduino)
                    time.sleep(2) #Wachttijd om nieuwe data op te halen
                else :
                    pass

    def getArduinoFromList(self):
        for i in self.arduinolist:
            return i

    def getTempSensorFromList(self):
        for i in self.arduinolist:
            if i.getTypeModel() == "TempSensor":
                return i

    def getLichtSensorFromList(self):
        for i in self.arduinolist:
            if i.getTypeModel() == "LightSensor":
                return i
    
    def haalTemperatuur(self): #Methode om temperatuur om de 60 seconden te updaten in het frame van Unit1
        i = self.getTempSensorFromList()
        temperatuurdata = i.receiveData()
        temperatuurinc = round(i.convertToC(int(temperatuurdata[4:7])), 2) #Temperatuur is de data die door de formule gaat om Celsisus te berekenen, temperatuurdata[4:7] als int met een round van 2 decimalen
        i.setTemperature(temperatuurinc) #Geef de temperatuur waarde door aan de private variabele van de arduino
        self.frames[Unit1].acttemplabel.config(text=f"Huidige temperatuur unit: {i.getTemperature()} C°")
        self.after(60000, self.haalTemperatuur)

    def getMinEntryButton(self): #Methode om het tekstveld op te halen
            intgetal = float(self.frames[Unit1].mintempentry.get()) * 10
            intgetal = int(round(intgetal))
            return intgetal

    def getMaxEntryButton(self): #Methode om het tekstveld op te halen
            intgetal = float(self.frames[Unit1].maxtempentry.get()) * 10
            intgetal = int(round(intgetal))
            return intgetal

   
class Unit1(tk.Frame):

    def __init__(self, parent, controller):
        self.isConnected = False
        self.showtemperature = 0

        tk.Frame.__init__(self,parent)
        button1 = tk.Button(self, text="Unit 1", 
                            command=lambda: controller.show_frame(Unit1))
        button1.grid(row = 0, column = 0, pady = 10, padx = 20)
        button2 = tk.Button(self, text="Unit 2", 
                            command=lambda: controller.show_frame(Unit2))
        button2.grid(row = 1, column = 0, pady = 10, padx = 20)
        labelinstellingen = tk.Label(self, text="Instellingen").grid(row=0, column = 4)
        minhuidiglabel = tk.Label(self, text=f"Huidige minimumtemperatuur: {round(controller.getTempSensorFromList().getMinValue(), 2)}") #Haalt minimum temperatuur van unit op
        minhuidiglabel.grid(row=1, column = 1, pady = 10, padx = 20)
        self.maxtemplabel = tk.Label(self, text=f"Huidige maximumtemperatuur: {round(controller.getTempSensorFromList().getMaxValue(), 2)}") # Haalt maximum temperatuur van unit op
        self.maxtemplabel.grid(row=2, column = 1, pady = 10, padx = 20)
        self.mintemplabel = tk.Label(self, text="Minimum temperatuur:").grid(row=1, column = 3)
        self.mintempentry = tk.Entry(self)
        self.mintempentry.grid(row=1, column = 4)
        mintempbutton = tk.Button(self, text="Minimum temperatuur opslaan", command= lambda: controller.getTempSensorFromList().sendMinSetting(controller.getMinEntryButton()))
        mintempbutton.grid(row = 1, column = 5, padx = 10)
        self.maxtemplabel = tk.Label(self, text="Maximum temperatuur:").grid(row=2, column = 3)
        self.maxtempentry = tk.Entry(self)
        self.maxtempentry.grid(row=2, column = 4)
        maxtempbutton = tk.Button(self, text="Maximum temperatuur opslaan", command= lambda: controller.getTempSensorFromList().sendMaxSetting(controller.getMaxEntryButton()))
        maxtempbutton.grid(row = 2, column = 5, padx = 10)
        self.acttemplabel = tk.Label(self, text=f"Huidige temperatuur: {self.showtemperature}") #Haalt minimum temperatuur van unit op
        self.acttemplabel.grid(row=0, column = 2, pady = 10, padx = 20)
        buttonomhoog = tk.Button(self, text="Scherm omhoog").grid(row = 4, column = 2, pady = 10)
        buttonomlaag = tk.Button(self, text="Scherm omlaag").grid(row = 4, column = 3, pady = 10)
        buttonled1 = tk.Button(self, text="LED1", command=lambda: controller.getTempSensorFromList().sendcommand('1'))
        buttonled1.grid(row = 5, column = 1, pady = 10, padx = 5)
        buttonled2 = tk.Button(self, text="LED2", command=lambda: controller.getTempSensorFromList().sendcommand('2'))
        buttonled2.grid(row = 5, column = 2, pady = 10, padx = 5)
        buttonled3 = tk.Button(self, text="LED3", command=lambda: controller.getTempSensorFromList().sendcommand('3'))
        buttonled3.grid(row = 5, column = 3, pady = 10, padx = 5)
        buttonled4 = tk.Button(self, text="LED4", command=lambda: controller.getTempSensorFromList().sendcommand('4'))     
        buttonled4.grid(row = 5, column = 4, pady = 10, padx = 5)
        buttonled5 = tk.Button(self, text="LEDUIT", command=lambda: controller.getTempSensorFromList().sendcommand('0'))
        buttonled5.grid(row = 5, column = 5, pady = 10, padx = 5)



class Unit2(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        button1 = tk.Button(self, text="Unit 1", 
                            command=lambda: controller.show_frame(Unit1))
        button1.grid(row = 0, column = 0)
        button2 = tk.Button(self, text="Unit 2", 
                            command=lambda: controller.show_frame(Unit2))
        button2.grid(row = 1, column = 0)
        labelinstellingen = tk.Label(self, text="Instellingen").grid(row=0, column = 3)
        mintemplabel = tk.Label(self, text="Minimum licht:").grid(row=1, column = 2)
        mintempentry = tk.Entry(self).grid(row=1, column = 3)
        maxtemplabel = tk.Label(self, text="Maximum licht:").grid(row=2, column = 2)
        maxtempentry = tk.Entry(self).grid(row=2, column = 3)
        opslaanbutton = tk.Button(self, text="Instellingen opslaan").grid(row = 3, column = 3)
        buttonomhoog = tk.Button(self, text="Scherm omhoog").grid(row = 4, column = 2, pady = 10)
        buttonomlaag = tk.Button(self, text="Scherm omlaag").grid(row = 4, column = 3, pady = 10)

    
app = MainUI()
app.sluitUnitAan()
app.title("Dashboard Zeng Ltd. zonnescherm")
app.haalTemperatuur()
app.mainloop()