import socket
import logging
from PyQt5.QtCore import QObject, pyqtSignal

from program_error import ProgramError

class ClientConnection(QObject):
    
    families = (socket.AF_INET, socket.AF_INET6)
    logInfo = pyqtSignal(str)                       # used to print messages in main window log
    
    def __init__(self):
        super().__init__()
        self._connected = False
        self._sock = None

    def getSocket(self):
        return self._sock

    def connect(self, host, port, ipv):
        if self._connected:
            raise ProgramError('Already connected')
        
        family = self.families[ipv%2]
        
        self.logInfo.emit(f'connecting to {host} port {port}   {repr(family)}')
        
        server_addr = (host, port)
        
        try:
            self._sock = socket.socket(family, socket.SOCK_STREAM)
            self._sock.connect(server_addr)
        except OSError as ex:
            self._sock = None
            raise ProgramError(f'Connection failed. {ex}')
            return
        
        self._connected = True
        
        
    def disconnect(self):
        if not self._connected:
            return

        logging.debug('sock.shutdown')        
        
        try:
            self._sock.shutdown(socket.SHUT_RDWR)
        except OSError as ex:
            self.logInfo.emit(f'sock.shutdown failed. {ex}')
        
        logging.debug('sock.close')        
        
        try:
            self._sock.close()
        except OSError as ex:
            self.logInfo.emit(f'sock.close failed. {ex}')
        
        self._sock = None
        self._connected = False
        
    def send(self, data):
        if not self._connected:
            raise ProgramError('Not connected')                            

        try:
            self._sock.sendall(data)                               
        except OSError as ex:
            raise ProgramError(f'Send failed. {ex}')                 
                               
