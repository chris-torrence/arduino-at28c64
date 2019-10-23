# Read in an Apple II ROM, do some optional modifications,
# then write out the bytes as an Arduino header file
# Chris Torrence, May 2016
from convert_roms_arduino import *

dir = '/Users/chris/AppleII/ROMs/'
inputFile = 'AppleIIPlus-341-0020-ApplesoftBasicAutostartMonitorF800-2716.bin'
# inputFile = 'Apple IIe CD Enhanced - 342-0304-A - 2764.bin'
f = open(dir + inputFile, 'rb')
data = f.read()
f.close()
data = np.fromstring(data,dtype=np.uint8)
print(dir + inputFile)

# Hack adapted from the Videx Enhancer ][ manual, p. A-4

#   FD83: AND #$DF  --> AND #$FF
data[0xFD83 - 0xF800] = 0xFF

#
#   FD11: AND #$3F  --> JSR $FBB4
#   FD13: ORA #$40  --> NOP
i = 0xFD11 - 0xF800
data[i:i+4] = [0x20, 0xB4, 0xFB, 0xEA]

#
#   FBB4: AND #$7F
#         CMP #$20  ; <space> character
#         BNE SKIP
#         ORA #$40  ; turn on flashing for <space>
#   SKIP: RTS
i = 0xFBB4 - 0xF800
new = [0x29, 0x7F, 0xC9, 0x20, 0xD0, 0x02, 0x09, 0x40, 0x60]
data[i:i+len(new)] = new

writeHeaderFile(inputFile, 'ProgData.h', data)
