import logging
from threading import Thread

from message import Message

# Python thread.
# Starts when client is connected. Stops when socket is disconnected.
# Reads socket and sends messages to MessageQueue, which redirects them to main window.

class ReadThread:
    def __init__(self, queue):
        self._sock = None
        self._thread = None
        self._queue = queue
    
    def start(self, sock):
        self._sock = sock
        self._thread = Thread(target=self.thread_function)
        self._thread.start()

    def thread_function(self):

        logging.info('thread function started')
        self._queue.put((Message.Text, 'recv thread started'))      # just to test text message
        
        while True:
            try:
                logging.debug('recv started')
                data = self._sock.recv(1024)
                logging.debug('recv ended')
            except OSError as ex:
                logging.error(f'recv failed. {ex}')
                break
            
            if data:
                logging.info(f'received {len(data)} bytes')
                self._queue.put((Message.Data, data))
            else:
                logging.info('recv returned None')
                break

        logging.info('thread function ended')
        self._queue.put((Message.Text, 'recv thread ended'))
        
    def join(self):
        
        if self._thread is None:
            logging.info('thread is None')
            return
            
        try:
            logging.debug('thread.join started')
            self._thread.join()
            logging.debug('thread.join ended')
        except OSError as ex:
            logging.error(f'hread.join failed. {ex}')
        
        self.thread = None
        
        
