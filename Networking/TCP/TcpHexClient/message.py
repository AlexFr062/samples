from enum import Enum

# Command - used as first tuple member in inter-thread communications.
class Message(Enum):
    Text = 1            # tuple[1] - message text
    Data = 2            # tuple[1] - data received
