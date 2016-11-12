from Ui_Form_startup import Ui_Form_startup
from PyQt5.QtWidgets import *


class StartUpView(QWidget, Ui_Form_startup):
    def __init__(self):
        super(QWidget, self).__init__()
        super(Ui_Form_startup, self).__init__()
        self.setupUi(self)
