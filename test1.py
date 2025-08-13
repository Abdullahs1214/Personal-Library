from PyQt6.QtCore import QProcess, QUrl
from PyQt6.QtGui import QWindow
from PyQt6.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton

class MyApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setGeometry(500, 500, 800, 600)
        self.setWindowTitle('Embedded BrainBay')

        layout = QVBoxLayout()
        self.setLayout(layout)

        self.launch_button = QPushButton('Launch BrainBay', self)
        self.launch_button.clicked.connect(self.launchBrainBay)
        layout.addWidget(self.launch_button)

        self.process = QProcess(self)

    def launchBrainBay(self):
        address_windows2 = "C:\\Users\\srskh\\AppData\\Local\\BrainBay\\brainBay.exe"
        self.process.start(address_windows2)

        # Embed BrainBay window
        if self.process.waitForStarted():
            self.brainbay_window = QWindow.fromWinId(self.process.processId())
            self.brainbay_window.setFlags(self.brainbay_window.flags() | Qt.WindowFlags.SubWindow)
            self.brainbay_window.create()

            # Add BrainBay window to layout
            self.layout().addWidget(self.brainbay_window)

if __name__ == '__main__':
    app = QApplication([])
    window = MyApp()
    window.show()
    app.exec()
