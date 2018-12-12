from PyQt5.QtCore import pyqtSlot
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import *
import Bridge
from threading import Thread
import time


app = QApplication([])

widget = QWidget()

vbox = QVBoxLayout()


label = QLabel('Hello World!')
vbox.addWidget(label)

def set_x(value): Bridge.set_x(value)


slider = QSlider(Qt.Horizontal)
slider.setSingleStep(1)
slider.setTickInterval(10)

slider.valueChanged.connect(set_x)


vbox.addWidget(slider)

widget.setLayout(vbox)
widget.show()

quit = False
def update() :
    global quit

    while (quit == False) :
        label.setText(str(Bridge.get_x()))
        time.sleep(.1)

t = Thread(target=update)
t.start()

app.exec_()
quit = True