from PyQt5.QtWidgets import * #QApplication, QWidget, QMainWindow, QStackedWidget
from StartupView.StartUpView import StartUpView


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.pages = QStackedWidget()
        self.setCentralWidget(self.pages)
        self.__add_ui()
        #self.window_setup()
        #self.set_layout()

    def __add_ui(self):
        ui_list = [StartUpView]
        for i, Class_name in enumerate(ui_list):
            view = Class_name()
            self.pages.addWidget(view)

    def set_layout(self):
        layout = QVBoxLayout(self.pages.currentWidget())
        layout.setSizeConstraint(QLayout.SetNoConstraint)
        self.setLayout(layout)

    def window_setup(self):
        pass
        #self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
