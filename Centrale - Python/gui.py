import tkinter as tk
from tkinter import *
import sys
sys.path.append("..") # Path zetten voor imports
from CentraleUnit import CentraleUnit # Import van CentraleUnit klasse
import time
import serial
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.animation as animation

portnames = {"COM0", "COM1", "COM2", "COM3", "COM4", "COM5"}

f = Figure(figsize=(5,4), dpi=100)
a = f.add_subplot(111)

yar = [18] 
xar = [0]


class MainUI(tk.Tk):
    def __init__(self, *args, **kwargs):
        
        self.arduinolist = [] #Lijst waar de arduino's in komen te staan
        self.isRunning = True # Boolean om aan te geven of het programma nog draait
        self.sluitUnitAan()
        self.unit1temperatuur = 0
        self.f = Figure(figsize=(5,4), dpi=100)
        self.a = f.add_subplot(111)
        self.xar = [0]
        self.yar = [0]
                   
        tk.Tk.__init__(self, *args, **kwargs)
        
        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)
        
        self.frames = {}

        for F in (Menu, Unit1, Unit2):

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(Menu)

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
    
    def checkUnit1(self):
        for i in self.arduinolist:
            if i.getTypeModel() == "TempSensor":
                self.frames[Menu].set1Connected()
                self.frames[Menu].updateMenu()
    
    def checkUnit2(self):
        for i in self.arduinolist:
            if i.getTypeModel() == "LightSensor":
                self.frames[Menu].set2Connected()
                self.frames[Menu].updateMenu()

    def haalTemperatuur(self): #Methode om temperatuur om de 60 seconden te updaten in het frame van Unit1
        i = self.getTempSensorFromList()
        temperatuurdata = i.receiveData()
        temperatuurdata = i.filterString(temperatuurdata) #filter eventuele rommel uit de string
        temperatuurinc = round(i.convertToC(int(temperatuurdata.split(",")[1])), 2) #Temperatuur is de data die door de formule gaat om Celsisus te berekenen, 2e item in de list als int met een round van 2 decimalen
        self.unit1temperatuur = temperatuurinc
        i.setTemperature(temperatuurinc) #Geef de temperatuur waarde door aan de private variabele van de arduino
        self.frames[Unit1].acttemplabel.config(text=f"Huidige temperatuur unit: {i.getTemperature()} C°")
        self.after(6000, self.haalTemperatuur)

    def getMinEntryButton(self): #Methode om het tekstveld op te halen
            intgetal = float(self.frames[Unit1].mintempentry.get()) * 10
            intgetal = int(round(intgetal))
            print(intgetal)
            return intgetal

    def getMaxEntryButton(self): #Methode om het tekstveld op te halen
            intgetal = float(self.frames[Unit1].maxtempentry.get()) * 10
            intgetal = int(round(intgetal))
            print(intgetal)
            return intgetal
    
    def animate(self):
        self.a.clear()
        self.yar.append(float(self.unit1temperatuur)) # gooit de temperatuur in de y array
        self.xar.append(self.xar[-1] + 60)
        print(self.xar)
        print(self.yar)
        self.a.plot(self.xar,self.yar)
        self.frames[Unit1].drawCanvas()
        self.after(60000, self.animate)

class Menu(tk.Frame):

    def __init__(self, parent, controller):
        self.unit1isconnected = False
        self.unit2isconnected = False
        self.controller = controller
        
        tk.Frame.__init__(self,parent)

    def set1Connected(self):
        self.unit1isconnected = True

    def set2Connected(self):
        self.unit1isconnected = True

    def updateMenu(self):
        if self.unit1isconnected == True:
            button1 = tk.Button(self, text="Unit 1", 
                                command=lambda: self.controller.show_frame(Unit1))
            button1.grid(row = 3, column = 0, pady = 10, padx = 20)
        if self.unit2isconnected == True:
            button2 = tk.Button(self, text="Unit 2", 
                                command=lambda: self.controller.show_frame(Unit2))
            button2.grid(row = 3, column = 1, pady = 10, padx = 20)
        elif  self.unit1isconnected == False and self.unit2isconnected == False:
            niksaangesloten = tk.Label(self, text="Er is op dit moment geen Arduino aangesloten op de centrale").grid(row=0, column = 0)

class Unit1(tk.Frame):

    def __init__(self, parent, controller):
        self.isConnected = False
        self.showtemperature = 0
        self.fdinkie = controller.f

        tk.Frame.__init__(self,parent)
        button1 = tk.Button(self, text="Terug naar Menu", 
                            command=lambda: controller.show_frame(Menu))
        button1.grid(row = 0, column = 0, pady = 5, padx = 10)
        #labelinstellingen = tk.Label(self, text="Instellingen").grid(row=0, column = 4)
        minhuidiglabel = tk.Label(self, text=f"Huidige minimumtemperatuur: {round(controller.getTempSensorFromList().getMinValue(), 2)}") #Haalt minimum temperatuur van unit op
        minhuidiglabel.grid(row=1, column = 0, pady = 5, padx = 10)
        self.maxtemplabel = tk.Label(self, text=f"Huidige maximumtemperatuur: {round(controller.getTempSensorFromList().getMaxValue(), 2)}") # Haalt maximum temperatuur van unit op
        self.maxtemplabel.grid(row=2, column = 0, pady = 5, padx = 10)
        self.mintemplabel = tk.Label(self, text="Minimum temperatuur:").grid(row=1, column = 1)
        self.mintempentry = tk.Entry(self)
        self.mintempentry.grid(row=1, column = 2)
        mintempbutton = tk.Button(self, text="Minimum temperatuur opslaan", command= lambda: controller.getTempSensorFromList().sendMinSetting(controller.getMinEntryButton()))
        mintempbutton.grid(row = 1, column = 3, padx = 5)
        self.maxtemplabel = tk.Label(self, text="Maximum temperatuur:").grid(row=2, column = 1)
        self.maxtempentry = tk.Entry(self)
        self.maxtempentry.grid(row=2, column = 2)
        maxtempbutton = tk.Button(self, text="Maximum temperatuur opslaan", command= lambda: controller.getTempSensorFromList().sendMaxSetting(controller.getMaxEntryButton()))
        maxtempbutton.grid(row = 2, column = 3, padx = 5)
        self.acttemplabel = tk.Label(self, text=f"Huidige temperatuur: {self.showtemperature}") #Haalt minimum temperatuur van unit op
        self.acttemplabel.grid(row=5, column = 0, pady = 5, padx = 10)
        self.buttonomhoog = tk.Button(self, text="Scherm omhoog", command=lambda: controller.getTempSensorFromList().sendcommand('1'))
        self.buttonomhoog.grid(row = 4, column = 3, pady = 5)
        self.buttonomlaag = tk.Button(self, text="Scherm omlaag", command=lambda: controller.getTempSensorFromList().sendcommand('2'))
        self.buttonomlaag.grid(row = 4, column = 2, pady = 5)
        #buttonled4 = tk.Button(self, text="LED4", command=lambda: controller.getTempSensorFromList().sendcommand('4'))     
        #buttonled4.grid(row = 5, column = 4, pady = 10, padx = 5)
        #buttonled5 = tk.Button(self, text="LEDUIT", command=lambda: controller.getTempSensorFromList().sendcommand('0'))
        #buttonled5.grid(row = 5, column = 5, pady = 10, padx = 5)

        canvas = FigureCanvasTkAgg(f, self)
        canvas.draw()
        canvas.get_tk_widget().grid(row = 6, column = 0)
        

    def setConnected(self):
        self.isConnected = True



class Unit2(tk.Frame):

    def __init__(self, parent, controller):

        self.showtemperature = 0
        tk.Frame.__init__(self, parent)
        button1 = tk.Button(self, text="Terug naar Menu", 
                            command=lambda: controller.show_frame(Menu))
        button1.grid(row = 0, column = 0)
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
        self.buttonomhoog = tk.Button(self, text="Scherm omhoog", command=lambda: controller.getTempSensorFromList().sendcommand('1'))
        self.buttonomhoog.grid(row = 4, column = 4, pady = 10)
        self.buttonomlaag = tk.Button(self, text="Scherm omlaag", command=lambda: controller.getTempSensorFromList().sendcommand('2'))
        self.buttonomlaag.grid(row = 4, column = 3, pady = 10)
        buttonled4 = tk.Button(self, text="LED4", command=lambda: controller.getTempSensorFromList().sendcommand('4'))     
        buttonled4.grid(row = 5, column = 4, pady = 10, padx = 5)
        buttonled5 = tk.Button(self, text="LEDUIT", command=lambda: controller.getTempSensorFromList().sendcommand('0'))
        buttonled5.grid(row = 5, column = 5, pady = 10, padx = 5)

    
app = MainUI()
app.sluitUnitAan()
app.title("Dashboard Zeng Ltd. zonnescherm")
app.checkUnit1()
app.checkUnit2()
app.haalTemperatuur()
def animate(i):
    a.clear() # is nodig om ram te clearen
    yar.append(float(app.unit1temperatuur)) # gooit de temperatuur in de y array
    xar.append(xar[-1] + 6)
    print(xar)
    print(yar)
    a.plot(xar,yar)

ani = animation.FuncAnimation(f,animate, interval=6000)
app.mainloop()