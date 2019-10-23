# Read in an Apple II ROM, do some optional modifications,
# then write out the bytes as an Arduino header file
# Chris Torrence, May 2016
from convert_roms_arduino import *

dir = ''
inputFile = '/Users/chris/AppleII/Apple2o.rom'
inputFile = '/Users/chris/Desktop/BOOTI/tmp/FIRMWARE.BIN'
f = open(dir + inputFile, 'rb')
data = f.read()
f.close()
data = np.fromstring(data,dtype=np.uint8)
print(dir + inputFile)

if 0:
  data = data[-2048:]

  #   FBE5: make BELL frequency lower, shorter
  data[0xFBE5 - 0xF800] = 0x0E
  data[0xFBE3 - 0xF800] = 0x70

  #
  #   FF69: LDA #$AA  --> JMP $FF59
  i = 0xFD28 - 0xF800
  data[i:i+3] = [0xA9, 0xB0, 0xEA]
  # i = 0xFF69 - 0xF800
  # data[i:i+6] = [0xAD, 0x10, 0xC0, 0x4C, 0x6C, 0xFF]

writeHeaderFile(inputFile, 'ProgData.h', data)
