import sys
import logging 

from PyQt5.QtWidgets import QWidget, QApplication, QMainWindow, QPushButton, QVBoxLayout, QHBoxLayout
from PyQt5.QtWidgets import QPlainTextEdit, QLabel, QLineEdit, QComboBox
from PyQt5.QtCore import pyqtSlot
from queue import Queue

from settings import Settings
from program_error import ProgramError
from client_connection import ClientConnection
import functions
from read_thread import ReadThread
from message_queue import MessageQueue
from message import Message

# TCP client working with binary data.
#
# Examples of IPv6 address: fc00::1 fc00::2 etc. Subnet prefix 64.


loggingLevel = level=logging.INFO
#loggingLevel = level=logging.DEBUG              # print all
programName = 'TCP Hex Client'

class HexClientWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle(programName)

        self.editHostAddress = QLineEdit()      
        self.editPort = QLineEdit()
        self.comboIpv = QComboBox()
        self.buttonConnect = QPushButton('Connect')
        self.buttonDisconnect = QPushButton('Disconnect')
        self.buttonClear = QPushButton('Clear')
        self.editData = QLineEdit()
        self.buttonSend = QPushButton('Send')
        self.editLog = QPlainTextEdit()
        
        
        self.settings = Settings()
        self.connection = ClientConnection()
        self.messageQueue = MessageQueue(self.onThreadMessage)   
        self.readThread = ReadThread(self.messageQueue.getQueue())
        self.connected = False

        self.connection.logInfo.connect(self.onLogInfo)

        layout = self.createLayout()
        self.subscribe()
        self.initControls()
        self.setControlsState()
        
        widget = QWidget()
        widget.setLayout(layout)
        self.setCentralWidget(widget)
       
        # set initial size and position
        self.setFixedSize(600,400)
        self.move(300, 200)

    def onButtonSend(self):
        try:
            byteArray, normalizedString = functions.hexStringToBytes(self.editData.text())
        except ProgramError as ex:
            self.printLog(str(ex))
            return
        
        self.editData.setText(normalizedString)     # show normalized string
        self.settings.data = self.editData.text()   # keep normalized string in settings
        
        try:
            self.connection.send(byteArray)
        except ProgramError as ex:
            self.printLog(f'{ex}')
            return
        
        self.printLog(f'>> {functions.bytesToHexString(byteArray)}')

    def onButtonConnect(self):
        
        # Read data from dialog controls
        try:
            self.settings.fromRawData(self.editHostAddress.text(),
                                      self.editPort.text(),
                                      self.comboIpv.currentIndex())
        except ProgramError as ex:
            self.printLog(str(ex))
            return

        self.setControls()

        # Connect
        try:
            self.connection.connect(self.settings.host, self.settings.port, self.settings.ipv)
        except ProgramError as ex:
            self.printLog(str(ex))
            return

        self.readThread.start(self.connection.getSocket())
        
        self.connected = True
        self.setControlsState()
        self.printLog('connected')

    def onButtonDisconnect(self):
        
        self.disconnect()
        
        self.connected = False
        self.setControlsState()
        self.printLog('disconnected')
     
    def disconnect(self):                       # called when Disconnect is pressed and when form is closed
        if self.connected: 
            self.connection.disconnect()
            self.readThread.join()
        
    def onButtonClear(self):
        self.editLog.clear()
      
    @pyqtSlot(str)
    def onLogInfo(self, message):               # slot for ClientConnection to send log messages
        self.printLog(message)
      
    @pyqtSlot(Queue)
    def onThreadMessage(self, message):         # slot for MessageQueue to send log messages
        if message[0] == Message.Text:          # second tuple member is text
            self.printLog(message[1])
        else:                                   # Message.Data: second tuple member is data received
            self.printLog(f'<< {functions.bytesToHexString(message[1])}')
      
    def onDataChanged(self):
        self.settings.data = self.editData.text()
        
    def createLayout(self):
        layoutMain = QVBoxLayout()
        layoutMain.setSpacing(20)
        
        layoutServerAddress = QHBoxLayout()
        layoutServerAddress.addWidget(QLabel('Host address'))
        layoutServerAddress.addWidget(self.editHostAddress)
        
        layoutPort = QHBoxLayout()
        layoutPort.setSpacing(20)
        layoutPort.addWidget(QLabel('Port'))
        layoutPort.addWidget(self.editPort)
        layoutPort.addWidget(self.comboIpv)
        layoutPort.addWidget(self.buttonConnect)
        layoutPort.addWidget(self.buttonDisconnect)
        layoutPort.addWidget(self.buttonClear)

        layoutData = QHBoxLayout()
        layoutData.addWidget(self.editData)
        layoutData.addWidget(self.buttonSend)
        
        self.editLog.setReadOnly(True)
        self.editLog.setLineWrapMode(QPlainTextEdit.NoWrap)
        
        layoutMain.addLayout(layoutServerAddress)
        layoutMain.addLayout(layoutPort)
        layoutMain.addLayout(layoutData)
        layoutMain.addWidget(self.editLog)
        
        return layoutMain

    def subscribe(self):
        self.buttonClear.clicked.connect(self.onButtonClear)
        self.buttonConnect.clicked.connect(self.onButtonConnect)
        self.buttonDisconnect.clicked.connect(self.onButtonDisconnect)
        self.buttonSend.clicked.connect(self.onButtonSend)
        self.editData.textChanged.connect(self.onDataChanged)
     
    def initControls(self):
        self.comboIpv.addItem("IPv4 ", None)
        self.comboIpv.addItem("IPv6 ", None)
     
        self.setControls()
     
    def setControls(self):
        self.editHostAddress.setText(self.settings.host)
        self.editPort.setText(str(self.settings.port))
        self.comboIpv.setCurrentIndex(self.settings.ipv % 2)    # %2 - precaution (dirty)
        self.editData.setText(self.settings.data)
     
    def printLog(self, s):
        self.editLog.appendPlainText(s)
        
    def setControlsState(self):
        self.buttonConnect.setEnabled(not self.connected)
        self.buttonDisconnect.setEnabled(self.connected)
        self.buttonSend.setEnabled(self.connected)
    
    def closeEvent(self, *args, **kwargs):
        '''
        Virtual function
        '''
        
        self.settings.Save()
        self.disconnect()
           
        return QMainWindow.closeEvent(self, *args, **kwargs) 
    
    
if __name__ == '__main__':
    # logging format: time with ms, function, message
    logging.basicConfig(format="%(asctime)s.%(msecs)03d: %(funcName)s: %(message)s", level=loggingLevel,
                        datefmt="%H:%M:%S")
    
    logging.info(f'{programName} started')
    
    app = QApplication(sys.argv)
    win = HexClientWindow()
    win.show()
    sys.exit(app.exec_())
    
        


