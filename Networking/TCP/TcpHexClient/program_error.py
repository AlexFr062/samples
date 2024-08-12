
# Exception thrown by other classes
class ProgramError(Exception):
    def __init__(self, message):
        super().__init__(message)
