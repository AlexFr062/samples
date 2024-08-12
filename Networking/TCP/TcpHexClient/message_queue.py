import logging
from PyQt5.QtCore import QObject, pyqtSignal, pyqtSlot
from queue import Queue
from PyQt5.Qt import QThread


class MessageQueue(QObject):
    '''
    Queue reader running in Qt thread. 
    Every message read from the queue is resent to main window using connected slot.
    This class is used for sending messages from non-QT thread (ReadThread) 
    to Qt window.
    
    ReadThread --- queue --- MessageQueue  ---- signal, slot ---- main window.
    
    '''

    command = pyqtSignal(tuple)
    

    def __init__(self, commandHandler):
        '''
        Create message queue, start queue read thread.
        commandHandler - slot to connect to command signal.
        '''
        super().__init__()
        logging.info("MessageQueue created")
        
        # Queue — synchronized queue class. Queue item - list, see command.py 
        self._queue = Queue()
        
        # Qt thread for reading Queue.
        self.thread = QThread()
        self.moveToThread(self.thread)              # move QObject to QtThread 
        self.thread.started.connect(self.run)       # When thread starts, execute run
        self.command.connect(commandHandler)        # connect to caller's slot
        self.thread.start()                         # run forever
      
    def getQueue(self):  
        return self._queue

    @pyqtSlot()
    def run(self):
        '''
        Called when  self.thread started. Runs in QtThread.
        Reads messages from self.queue and sends them to slot connected to command.
        Exits when the whole program is closed.
        '''
        
        logging.info("thread function started")
        
        
        while True:
            item = self._queue.get()
            self.command.emit(item)
        
