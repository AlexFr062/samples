import logging

from program_error import ProgramError

# ****************************************************************************
# different functions
# ****************************************************************************



# Convert bytes or bytearray array to hex string which looks like:
# 01 F2 FF ...
def bytesToHexString(byteArray):
    return ''.join('{:02X} '.format(x) for x in byteArray)


# Convert hexadecimal string to bytes array.
# Returns: tuple with bytes object and normalized hex string.
#
# Examples:
# hexString         normalizedString
# 00 F1 12          00 F1 F2
# 0 1 2             00 01 01
# 01ff 2a           01 ff 2A
#
# On error throws ProgramError.
#
def hexStringToBytes(hexString):
    s = hexString.strip()               # Return a copy of the string with the leading and trailing characters removed
    
    if len(s) == 0:
        raise ProgramError('No data to send')
    
    substrings = s.split()              # Return a list of the words in the string
    
    # Normalize substrings: every substring must have even number of symbols,
    # otherwise bytearray.fromhex gives error.
    for i in range(0, len(substrings)):
        if len(substrings[i]) % 2 == 1:
            substrings[i] = f'0{substrings[i]}'
    
    logging.debug(f'substrings {substrings}')
    
    # str.join(iterable) - Return a string which is the concatenation of the strings in iterable.
    
    s1 = ''.join(substrings)        # odd number of symbols without spaces
    
    logging.debug(f's1 {s1}')
    
    try:
        byteArray = bytes.fromhex(s1)
        logging.debug(f'byteArray {byteArray}')
    except ValueError as ex:
        raise ProgramError(f'Incorrect data. {ex}')
        return
    
    # Make normalized string with every number containing 2 symbols, and space delimiter
    normalizedString = bytesToHexString(byteArray)
    
    return (byteArray, normalizedString)

