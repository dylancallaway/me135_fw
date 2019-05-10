import sys
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore, QtWidgets
import pyqtgraph as pg
import numpy as np

from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg

import collections
import random
import time
import math
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

global ser


class MainWindow(QMainWindow):

    def __init__(self, parent=None):

        super(MainWindow, self).__init__(parent)

        self.bh_score = 0
        self.u_score = 0
        self.diffscore = self.bh_score - self.u_score

        self.title = "Bair Hockey"
        self.initUI()

        global ser
        ser = serial.Serial(port='/dev/ttyS0', baudrate=115200)
        ser.flushOutput()
        # self.fig, self.ax = plt.subplots(1)

        # setting frameless window
        # flags = QtCore.Qt.WindowFlags(QtCore.Qt.FramelessWindowHint | QtCore.Qt.WindowStaysOnTopHint)
        # self.setWindowFlags(flags)

        # set fullscreen
        # avail_geom = QtGui.QDesktopWidget().availableGeometry()
        # self.setGeometry(avail_geom)

    def initUI(self):

        self.setWindowTitle(self.title)
        # self.showFullScreen()

        # setting up difficulty buttons
        self.OptionsWidget = QWidget(self)
        self.OptionsWidget.easybutton = QPushButton('Easy')
        self.OptionsWidget.easybutton.clicked.connect(self.easy)
        self.OptionsWidget.easybutton.resize(200, 100)
        self.OptionsWidget.mediumbutton = QPushButton('Medium')
        self.OptionsWidget.mediumbutton.clicked.connect(self.medium)
        self.OptionsWidget.mediumbutton.resize(200, 100)
        self.OptionsWidget.hardbutton = QPushButton('Hard')
        self.OptionsWidget.hardbutton.clicked.connect(self.hard)
        self.OptionsWidget.hardbutton.resize(200, 100)
        self.OptionsWidget.startbutton = QPushButton('Start')
        self.OptionsWidget.startbutton.clicked.connect(self.start)
        self.OptionsWidget.startbutton.resize(200, 100)
        self.OptionsWidget.stopbutton = QPushButton('Stop')
        self.OptionsWidget.stopbutton.clicked.connect(self.stop)
        self.OptionsWidget.stopbutton.resize(200, 100)

        # set options VBox
        options_layout = QVBoxLayout()
        options_layout.addWidget(self.OptionsWidget.easybutton)
        options_layout.addWidget(self.OptionsWidget.mediumbutton)
        options_layout.addWidget(self.OptionsWidget.hardbutton)
        options_layout.addWidget(self.OptionsWidget.startbutton)
        options_layout.addWidget(self.OptionsWidget.stopbutton)
        self.OptionsWidget.setLayout(options_layout)

        # initializing scoreboard widget
        self.ScoreWidget = QWidget(self)
        # user score
        self.ScoreWidget.u_scoreplus = QPushButton('+')
        self.ScoreWidget.u_scoreplus.clicked.connect(self.u_add)
        self.ScoreWidget.u_scoreminus = QPushButton('-')
        self.ScoreWidget.u_scoreminus.clicked.connect(self.u_sub)
        self.ScoreWidget.u_scorelab = QLabel("Player: 0")
        # self.ScoreWidget.u_scorelab.setFont(Qfont("Comic Sans",20))

        # Bair hockey score
        self.ScoreWidget.bh_scoreplus = QPushButton('+')
        self.ScoreWidget.bh_scoreplus.clicked.connect(self.bh_add)
        self.ScoreWidget.bh_scoreminus = QPushButton('-')
        self.ScoreWidget.bh_scoreminus.clicked.connect(self.bh_sub)
        self.ScoreWidget.bh_scorelab = QLabel("Bair Hockey: 0")
        # self.ScoreWidget.bh_scorelab.setFont(Qfont("Ubuntu Mono",20))

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
        # self.showFullscreen()

    def location_on_the_screen(self):
        screen = QDesktopWidget().screenGeometry()
        main = self.geometry()
        self.puck_x = screen.width() - main.width()
        self.puck_y = screen.height() - main.height()
        self.move(self.puck_x, self.puck_y)

    def easy(self):
        diff = b'0'
        # print(diff)
        print(ser.write(diff))

    def medium(self):
        diff = b'1'
        # print(diff)
        ser.write(diff)

    def hard(self):
        diff = b'2'
        # print(diff)
        ser.write(diff)

    def start(self):
        init = b'3'
        # print("start")
        ser.write(init)

    def stop(self):
        stop = b'4'
        # print("start")
        ser.write(stop)

    def u_add(self):
        self.u_score += 1
        if self.u_score >= 7:
            self.result()
            self.u_score = 0
        self.ScoreWidget.u_scorelab.setText("Player: " + str(self.u_score))
        # print("you win")

    def u_sub(self):
        self.u_score -= 1
        if self.u_score < 0:
            self.u_score = 0
        self.ScoreWidget.u_scorelab.setText("Player: " + str(self.u_score))

    def bh_add(self):
        self.bh_score += 1
        if self.bh_score >= 7:
            self.result()
            self.bh_score = 0
        self.ScoreWidget.bh_scorelab.setText(
            "Bair Hockey: " + str(self.bh_score))

    def bh_sub(self):
        self.bh_score -= 1
        if self.bh_score < 0:
            self.bh_score = 0
        self.ScoreWidget.bh_scorelab.setText(
            "Bair Hockey: " + str(self.bh_score))

    def result(self):
        if self.u_score >= 7:
            result_PopUp = WinnerPopUp(self)
            print('you win')
        elif self.bh_score >= 7:
            result_PopUp = LoserPopUp(self)

        result_PopUp.show()


class WinnerPopUp(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.name = "You won!"
        self.label = QLabel(self.name, self)
        self.resize(200, 200)


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
       # self.setGeometry(self.left, self.top, self.width, self.height)
        # m = dyn_plot()
        # m.move(0,0)


# class dyn_plot(pg.GraphicsWindow):

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


class DynamicPlotter():

    def __init__(self, sampleinterval=0.1, timewindow=10., size=(600, 350)):
        # Data stuff
        global ser
        self.recv_buf = [0, 0, 0, 0]
        self._interval = int(sampleinterval*1000)
        self._bufsize = int(timewindow/sampleinterval)
        self.databuffer = collections.deque([0.0]*self._bufsize, self._bufsize)
        self.x = np.linspace(-timewindow, 0.0, self._bufsize)
        self.y = np.zeros(self._bufsize, dtype=np.float)
        # PyQtGraph stuff
        self.app = QtGui.QApplication([])
        self.plt = pg.plot(title='BairHockey')
        self.plt.resize(*size)
        self.plt.showGrid(x=True, y=True)
        self.plt.setLabel('left', 'Puck Y Position', 'px')
        self.plt.setLabel('bottom', 'Time', 's')
        self.curve = self.plt.plot(self.x, self.y, pen=(255, 0, 0))
        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.getData)
        self.timer.start(self._interval)

    def getdata(self):
        frequency = 0.5
        noise = random.normalvariate(0., 1.)
        new = 10.*math.sin(time.time()*frequency*2*math.pi) + noise
        return new

    def updateplot(self):
        self.databuffer.append(self.getdata())
        self.y[:] = self.databuffer
        self.curve.setData(self.x, self.y)
        self.app.processEvents()

    def getData(self):
        if ser.inWaiting() > 3:
            buf = ser.read(4)
            ser.flushInput()
            int_buf = int.from_bytes(buf, byteorder='big')
            val = self.conv(int_buf)
            self.databuffer.append(val)
            self.y[:] = self.databuffer
            self.curve.setData(self.x, self.y)
            self.app.processEvents()

    def conv(self, data):
        data = data % 4294967296
        self.puck_y = data % 256
        data = data/256
        self.puck_x = data % 256
        data = data/256
        self.pad_y = data % 256
        self.pad_x = data/256
        if self.puck_x > 250:
            self.puck_x = 0
        if self.puck_y > 250:
            self.puck_y = 0
        return self.puck_y

    def run(self):
        self.app.exec_()


if __name__ == '__main__':

    app = QApplication(sys.argv)

    main = MainWindow()
    main.location_on_the_screen()
    main.show()

    # getData()
    # dyn_plot()
    # plt.show()

    # timer = QtCore.QTimer()
    # timer.setInterval(1)  # in milliseconds
    # timer.timeout.connect(main.getData)
    # timer.start()

    m = DynamicPlotter(sampleinterval=0.05, timewindow=10.)
    m.run()

    sys.exit(app.exec_())
