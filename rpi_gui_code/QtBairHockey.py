import sys
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore, QtWidgets
import pyqtgraph as pg
import numpy as np

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.figure import Figure

import serial
import time

from pyqtgraph.Qt import QtGui, QtCore


import collections
import random
import math




class MainWindow(QMainWindow):

    def __init__(self, parent=None):

        super(MainWindow, self).__init__(parent)

        self.bh_score = 0
        self.u_score = 0
        self.diffscore = self.bh_score - self.u_score

        self.title = "Bair Hockey"
        self.initUI()

        #setting frameless window
        #flags = QtCore.Qt.WindowFlags(QtCore.Qt.FramelessWindowHint | QtCore.Qt.WindowStaysOnTopHint)
        #self.setWindowFlags(flags)

        #set fullscreen
        #avail_geom = QtGui.QDesktopWidget().availableGeometry()
        #self.setGeometry(avail_geom)

    def initUI(self):

        self.setWindowTitle(self.title)
        #self.showFullScreen()

	#setting up difficulty buttons
        self.OptionsWidget = QWidget(self)
        self.OptionsWidget.easybutton = QPushButton('Easy')
        self.OptionsWidget.easybutton.clicked.connect(self.easy)
        self.OptionsWidget.easybutton.resize(200,100)
        self.OptionsWidget.mediumbutton = QPushButton('Medium')
        self.OptionsWidget.mediumbutton.clicked.connect(self.medium)
        self.OptionsWidget.mediumbutton.resize(200,100)
        self.OptionsWidget.hardbutton = QPushButton('Hard')
        self.OptionsWidget.hardbutton.clicked.connect(self.hard)
        self.OptionsWidget.hardbutton.resize(200,100)
        self.OptionsWidget.startbutton = QPushButton('Start')
        self.OptionsWidget.startbutton.clicked.connect(self.start)
        self.OptionsWidget.startbutton.resize(200,100)
        self.OptionsWidget.stopbutton = QPushButton('Stop')
        self.OptionsWidget.stopbutton.clicked.connect(self.stop)
        self.OptionsWidget.stopbutton.resize(200,100)


        #set options VBox
        options_layout = QVBoxLayout()
        options_layout.addWidget(self.OptionsWidget.easybutton)
        options_layout.addWidget(self.OptionsWidget.mediumbutton)
        options_layout.addWidget(self.OptionsWidget.hardbutton)
        options_layout.addWidget(self.OptionsWidget.startbutton)
        options_layout.addWidget(self.OptionsWidget.stopbutton)
        self.OptionsWidget.setLayout(options_layout)

        #initializing scoreboard widget
        self.ScoreWidget = QWidget(self)
        #user score
        self.ScoreWidget.u_scoreplus = QPushButton('+')
        self.ScoreWidget.u_scoreplus.clicked.connect(self.u_add)
        self.ScoreWidget.u_scoreminus = QPushButton('-')
        self.ScoreWidget.u_scoreminus.clicked.connect(self.u_sub)
        self.ScoreWidget.u_scorelab = QLabel("Player: 0")
        #self.ScoreWidget.u_scorelab.setFont(Qfont("Comic Sans",20))

        #Bair hockey score
        self.ScoreWidget.bh_scoreplus = QPushButton('+')
        self.ScoreWidget.bh_scoreplus.clicked.connect(self.bh_add)
        self.ScoreWidget.bh_scoreminus = QPushButton('-')
        self.ScoreWidget.bh_scoreminus.clicked.connect(self.bh_sub)
        self.ScoreWidget.bh_scorelab = QLabel("Bair Hockey: 0")
        #self.ScoreWidget.bh_scorelab.setFont(Qfont("Ubuntu Mono",20))

        scoreboard_layout = QVBoxLayout()
        scoreboard_layout.addWidget(self.ScoreWidget.u_scoreplus)
        scoreboard_layout.addWidget(self.ScoreWidget.u_scoreminus)
        scoreboard_layout.addWidget(self.ScoreWidget.u_scorelab)
        scoreboard_layout.addWidget(self.ScoreWidget.bh_scoreplus)
        scoreboard_layout.addWidget(self.ScoreWidget.bh_scoreminus)
        scoreboard_layout.addWidget(self.ScoreWidget.bh_scorelab)
        self.ScoreWidget.setLayout(scoreboard_layout)

        self.MainGrid = QGridLayout()
        self.MainGrid.addWidget(self.OptionsWidget)
        self.MainGrid.addWidget(self.ScoreWidget)

        self.MainWidget = QWidget(self)
        self.MainWidget.setLayout(self.MainGrid)
        self.setCentralWidget(self.MainWidget)
        self.show()
        #self.showFullscreen()

    def location_on_the_screen(self):
        screen = QDesktopWidget().screenGeometry()
        main = self.geometry()
        x = screen.width() - main.width()
        y = screen.height() - main.height()
        self.move(x, y)


    def easy(self):
        diff = b'0'
        #print(diff)
        self.sendData(diff)

    def  medium(self):
        diff = b'1'
        #print(diff)
        self.sendData(diff)

    def hard(self):
        diff = b'2'
        #print(diff)
        self.sendData(diff)


    def start(self):
        init = b'3'
        #print("start")
        self.sendData(init)


    def stop(self):
        print("stop this bullshit")
        stop = b'4'
        #print("start")
        self.sendData(stop)

    def u_add(self):
        self.u_score += 1
        self.ScoreWidget.u_scorelab.setText("Player: " + str(self.u_score))
        if self.u_score >= 7:
            self.result()
            #print("you win")

    def u_sub(self):
        self.u_score-= 1
        self.ScoreWidget.u_scorelab.setText("Player: " + str(self.u_score))
        if self.u_score >= 7:
            self.result()

    def bh_add(self):
        self.bh_score += 1
        self.ScoreWidget.bh_scorelab.setText("Bair Hockey: " + str(self.bh_score))
        if self.bh_score >= 7:
            self.result()

    def bh_sub(self):
        self.bh_score -= 1
        self.ScoreWidget.bh_scorelab.setText("Bair Hockey: " + str(self.bh_score))
        if self.bh_score >= 7:
            self.result()

    def result(self):
        if self.u_score >= 7:
            result_PopUp = WinnerPopUp(self)
            print('you win')
        elif self.bh_score >= 7:
            result_PopUp = LoserPopUp(self)

        result_PopUp.show()

    def sendData(self, sig):
        ser.write(sig)
        #time.sleep(1)

    def update_plot(self):
        self.DigiTablePlot.figure.clear()
        self.DigiTablePlot.canvas.draw()

class WinnerPopUp(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.name= "You won!"
        self.label = QLabel(self.name, self)
        self.resize(200,200)

class LoserPopUp(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.name = "You lost!"
        self.label = QLabel(self.name, self)

# class PlotWindow(QMainWindow):
#
    # def __init__(self):
        # super().__init__()
        # self.title= 'Live View'
        # self.initUI()
#
    # def initUI(self):
        # self.setWindowTitle(self.title)
       #self.setGeometry(self.left, self.top, self.width, self.height)
        # m = dyn_plot()
        # m.move(0,0)


#class dyn_plot(pg.GraphicsWindow):

#    def __init__(self, parent=None):

#        super().__init__(parent=parent)
#        self.mainLayout=QtWidgets.QVBoxLayout()
#        self.setLayout(self.mainLayout)
# class App(QMainWindow):
#
    # def __init__(self):
        # super().__init__(parent=None)
        # self.initUI()
    #
    # def initUI(self):
            #
        # self.setWindowTitle('Live Puck View')
        # m = dyn_plot(self)
        # m.move(0,0)
            #
        # self.show()
        #

def dyn_plot():

        x = 0
        y = 0
     
        #self.plotItem = self.addPlot(title="Live Puck View")
        #self.plotDataItem = self.plotItem.plot([], pen=None,symbolBrush=(255,0,0), symbolSize=15, symbolPen=None)

def getData():
    try:
        if ser.inWaiting() > 0:
            new_data = ser.read()
            return new_data
            #x = new_data[1]
            #y = new_data[2]
        else:
                new_data = [65,0]
    except NameError:
        print("Serial port not defined...must be in test mode.")
        new_data = [65, 0] 

    update_plot(new_data)


def update_plot(new_data):
    x = new_data[0]
    y = new_data[1]

    plt.plot(x, y)
    #plt.scatter(x, y, c="r")

    #def update_data(self, x, y):
    #    self.plotDataItem.setData(x,y)

if __name__ == '__main__':

    plt.ion()

    app=QApplication(sys.argv)
    # ser = serial.Serial(port = '/dev/ttyS0', baudrate = 115200)

    main=MainWindow()
    main.location_on_the_screen()
    main.show()

    # getData()
    # dyn_plot()
    # plt.show()

    timer = QtCore.QTimer()
    timer.setInterval(10) # in milliseconds
    timer.timeout.connect(getData)
    timer.start()


    
    sys.exit(app.exec_())
