import tkinter as tk
import sys
sys.path.append("..") # Path zetten voor imports
from CentraleUnit import CentraleUnit # Import van CentraleUnit klasse



class MainUI(tk.Tk):

    def __init__(self, *args, **kwargs):
        
        self.arduinolist = [] #Lijst waar de arduino's in komen te staan

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

    def sluitUnitAan(self): #Methode om Arduino aan te sluiten. 
        arduino = CentraleUnit("COM4", "TempSensor")
        self.arduinolist.append(arduino)

    def getArduinoFromList(self):
        for i in self.arduinolist:
            return i
            

        
class Unit1(tk.Frame):

    def __init__(self, parent, controller):
        self.isConnected = False

        tk.Frame.__init__(self,parent)
        button1 = tk.Button(self, text="Unit 1", 
                            command=lambda: controller.show_frame(Unit1))
        button1.grid(row = 0, column = 0)
        button2 = tk.Button(self, text="Unit 2", 
                            command=lambda: controller.show_frame(Unit2))
        button2.grid(row = 1, column = 0)
        labelinstellingen = tk.Label(self, text="Instellingen").grid(row=0, column = 3)
        mintemplabel = tk.Label(self, text="Minimum temperatuur:").grid(row=1, column = 2)
        mintempentry = tk.Entry(self).grid(row=1, column = 3)
        maxtemplabel = tk.Label(self, text="Maximum temperatuur:").grid(row=2, column = 2)
        maxtempentry = tk.Entry(self).grid(row=2, column = 3)
        opslaanbutton = tk.Button(self, text="Instellingen opslaan").grid(row = 3, column = 3)
        buttonomhoog = tk.Button(self, text="Scherm omhoog").grid(row = 4, column = 2, pady = 10)
        buttonomlaag = tk.Button(self, text="Scherm omlaag").grid(row = 4, column = 3, pady = 10)
        buttonled1 = tk.Button(self, text="LED1", command=lambda: controller.getArduinoFromList().sendcommand('1'))
        buttonled1.grid(row = 5, column = 1, pady = 5)
        buttonled2 = tk.Button(self, text="LED2", command=lambda: controller.getArduinoFromList().sendcommand('2'))
        buttonled2.grid(row = 5, column = 2, pady = 5)
        buttonled3 = tk.Button(self, text="LED3", command=lambda: controller.getArduinoFromList().sendcommand('3'))
        buttonled3.grid(row = 5, column = 3, pady = 5)
        buttonled4 = tk.Button(self, text="LED4", command=lambda: controller.getArduinoFromList().sendcommand('4'))
        buttonled4.grid(row = 5, column = 4, pady = 5)
        buttonled5 = tk.Button(self, text="LEDUIT", command=lambda: controller.getArduinoFromList().sendcommand('0'))
        buttonled5.grid(row = 5, column = 5, pady = 5)



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
app.mainloop()
