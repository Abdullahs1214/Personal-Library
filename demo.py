from PyQt6.QtWidgets import QMainWindow, QApplication, QWidget, QVBoxLayout, QLabel, QPushButton
from PyQt6.QtGui import QColor
from PyQt6.QtCore import Qt, QTimer, QDateTime

class ClockWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.label = QLabel("Clock Widget", self)
        self.label.setAlignment(Qt.AlignmentFlag.AlignCenter)

        layout = QVBoxLayout()
        layout.addWidget(self.label)
        self.setLayout(layout)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_label)
        self.timer.start(1000)

        self.is_24_hour_format = True

    def update_label(self):
        current_time = QDateTime.currentDateTime()
        if self.is_24_hour_format:
            current_time_str = current_time.toString("hh:mm:ss")
        else:
            current_time_str = current_time.toString("h:mm:ss AP")  # 12-hour format
        self.label.setText(current_time_str)

    def toggle_format(self):
        self.is_24_hour_format = not self.is_24_hour_format
        self.update_label()


class OverlayWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.setAutoFillBackground(True)
        p = self.palette()
        p.setColor(self.backgroundRole(), QColor(0, 255, 0))  # Green background
        self.setPalette(p)

        self.clock_widget = ClockWidget()
        self.toggle_button = QPushButton("Toggle Time Format")
        self.toggle_button.clicked.connect(self.clock_widget.toggle_format)

        layout = QVBoxLayout()
        layout.addWidget(self.clock_widget)
        layout.addWidget(self.toggle_button)
        self.setLayout(layout)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Overlay Example")
        self.setGeometry(100, 100, 300, 300)

        self.overlay_widget = OverlayWidget()
        self.setCentralWidget(self.overlay_widget)


app = QApplication([])
window = MainWindow()
window.show()
app.exec()
