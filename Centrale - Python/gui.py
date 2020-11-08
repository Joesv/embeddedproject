import tkinter as tk

class MainUI(tk.Tk):

    def __init__(self, *args, **kwargs):
        
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

        
class Unit1(tk.Frame):

    def __init__(self, parent, controller):
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
app.title("Dashboard Zeng Ltd. zonnescherm")
app.mainloop()