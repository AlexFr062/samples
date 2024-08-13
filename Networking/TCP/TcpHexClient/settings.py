import configparser
import logging

from program_error import ProgramError

# Application settings.
# When created, loads itself from configuration file.
# Saves itself to file by request.
class Settings:
    _fileName = './.hex_client.config'
    _section_name = 'settings' 
             
    # fields configuration
    _host_name = 'host'             # string
    _port_name = 'port'             # port, int
    _ipv_name = 'ipv'               # IP version, int  (0-IPv$, 1 - IPv6)
    _data_name = 'data'
    
    _host_default = 'localhost'
    
    _port_min = 0
    _port_max = 0xFFFF
    _port_default = 41000
    
    _ipv_min = 0
    _ipv_max = 1
    _ipv_default = _ipv_min  
    
    
    def __init__(self):
        self._load()

    def fromRawData(self, host, port, ipv):
        '''
        Validate raw data for connection fields. 
        If data is OK - change current instance and return True.
        On error throws ProgramException.
        Parameters:
        host, port - strings
        ipv - integer
        '''
        host = host.strip()
        if len(host) == 0:
            raise ProgramError('Empty host name')
        
        try:
            port = int(port)
        except ValueError as ex:
            raise ProgramError(f'Incorrect port: {ex}')
        
        if port < self._port_min or port > self._port_max:
            raise ProgramError(f'port {port} is out of range: {self._port_min}-{self._port_max}')
        
        if ipv < self._ipv_min or ipv > self._ipv_max:
            raise ProgramError(f'ipv {ipv} is out of range: {self._ipv_min}-{self._ipv_max}')

        # OK
        self.host = host
        self.port = port
        self.ipv = ipv

    def Save(self):
        config = configparser.ConfigParser()
        self._toConfig(config)

        logging.debug(f'saving settings to file {self._fileName}')
        
        try:
            with open(self._fileName, 'w') as configfile:
                config.write(configfile)
            
        except OSError as ex:
            logging.error(f'OSError writing file file {self._fileName} {ex}')
        
    
    def _load(self):
        logging.debug(f'loading settings from file {self._fileName}')
        
        config = configparser.ConfigParser()
        
        # If file cannot be open, read doesn't throw, just remains empty.
        # if fileName is not string (for example, None), throws TypeError. 
        # Empty config/incorrect info is handled in fromConfig function.
        config.read(self._fileName)
        
        self._fromConfig(config)
        
        logging.info(self)
    
    def _toConfig(self, configParser):
        '''
        Write to configparser
        '''
        configParser[self._section_name] = {
            self._host_name: self.host, 
            self._port_name: self.port,
            self._ipv_name: self.ipv,
            self._data_name : self.data }

    def _fromConfig(self, configParser):
        '''
        Read from  configparser
        '''
        try:
            section = configParser[self._section_name]
            self.host = section[self._host_name]
            port = section[self._port_name]
            ipv = section[self._ipv_name]
            self.data = section[self._data_name]
            
            # convert to required types    
            self.port = int(port)
            self.ipv = int(ipv)
            
        except KeyError as ex:
            logging.error(f'KeyError {ex}')
            self._setDefault()
            return
        except ValueError as ex:
            logging.error(f'ValueError {ex}')
            self._setDefault()
            return
        
        # ensure range
        if self.port < self._port_min or self.port > self._port_max:
            self.port = self._port_default

        if self.ipv < self._ipv_min or self.ipv > self._ipv_max:
            self.ipv = self._ipv_default
        
    def _setDefault(self):
        self.host = self._host_default
        self.port = self._port_default
        self.ipv = self._ipv_default
        self.data = ''

    def __str__(self):
        return f'{self._host_name} \"{self.host}\" {self._port_name} {self.port} {self._ipv_name} {self.ipv} {self._data_name} {self.data}'    
