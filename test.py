import subprocess
import sys
import win32gui
from PyQt6.QtWidgets import QMainWindow, QApplication, QVBoxLayout, QWidget, QPushButton
from PyQt6.QtCore import QProcess, Qt

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        button = QPushButton('Launch BrainBay', self)
        button.clicked.connect(self.launchBrainBay)

        widget = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(button)
        widget.setLayout(layout)
        self.setCentralWidget(widget)

        self.setGeometry(500, 500, 450, 400)
        self.setWindowTitle('Exercise 3')
        self.show()

    def launchBrainBay(self):
        address_windows2 = "C:\\Users\\srskh\\AppData\\Local\\BrainBay\\brainBay.exe"
        self.process = QProcess(self)
        self.process.started.connect(self.onProcessStarted)
        self.process.start(address_windows2)

    def onProcessStarted(self):
        process = self.sender()
        process.waitForWindowShown()
        hwnd = process.windowHandle().nativeWindowHandle()
        rect = win32gui.GetWindowRect(hwnd)
        width = rect[2] - rect[0]
        height = rect[3] - rect[1]
        self.resize(width, height)

    def closeEvent(self, event):
        if self.process is not None:
            self.process.kill()
        event.accept()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    sys.exit(app.exec())
