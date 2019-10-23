# CT, March 2018
# Combine the Apple //e and II+ character ROMs to create a new
# Apple II+ character ROM that has lowercase characters in $C0-$FF
# and inverse lowercase characters in the "flashing" range $60-$7F
# Use along with my hack for the system ROM where it displays lowercase
# characters correctly and where backspace uses inverse characters
# instead of flashing.
import numpy as np
from convert_roms_arduino import *

dir = '/Users/chris/AppleII/ROMs/'
inputFile = 'AppleIIeVideoEnhanced-342-0265-a-2732.bin'
f = open(dir + inputFile, 'rb')
dataIn = f.read()
f.close()
dataIn = np.fromstring(dataIn,dtype=np.uint8)

inputFile = 'AppleIIPlusCharacters.bin'
f = open(dir + inputFile, 'rb')
dataOut = f.read()
f.close()
dataOut = np.fromstring(dataOut,dtype=np.uint8)

# Copy the "solid Apple" character to the end of the inverse lowercase row.
# Be sure to invert the bits since the Apple //e char rom has a different format.
dataIn[0x3F8:0x400] = ~dataIn[0x200:0x208]

# Shift all Apple II+ characters up by 1 row so they line up with
# the new lowercase characters from the //e.
for char in range(0x0, 0x100):
  c = char*8
  firstRow = dataOut[c]
  if firstRow == 0 or firstRow == 128:
    dataOut[c:c+7] = dataOut[c+1:c+8]
    dataOut[c+7] = firstRow

for char in range(0x60, 0x80):
  for row in range(0,8):
    i = char*8 + row
    # //e and II+ have a different character rom format
    # Need to reverse the bit order and right shift by 1 bit
    character = int('{:08b}'.format(dataIn[i])[::-1], 2) >> 1
    if char != 0x60:
      dataOut[i] = character
    # Turn on the high bit for the "normal" lowercase $C0-$FF
    # Not strictly necessary (ignored by the II+) but matches the uppercase
    dataOut[i + 0x400] = character | 128

# Turn flashing off (the high bit) for all characters except "space" at 0x300
for i in range(0x200, 0x300):
  dataOut[i] = dataOut[i] & ~128

dataOut = dataOut.tostring()
file = dir + 'AppleIIPlusTorrenceLowercase.bin'
f = open(file, 'wb')
f.write(dataOut)
f.close()

drawCharacters(dataOut)
writeHeaderFile(file, 'ProgData.h', dataOut)
