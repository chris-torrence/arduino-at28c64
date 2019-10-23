# Read in an Apple II character generator ROM,
# make an image of the characters,
# then write out the bytes as an Arduino header file
# Chris Torrence, May 2016
import numpy as np
import matplotlib.pyplot as plt
from time import localtime, strftime

def drawCharacters(data):
  """
  Create a character map image from a Apple II character generator ROM.
  Each character is 8 bytes. Each byte is one line in the character,
  with each bit indicating an off/on pixel in that line, in reverse order.
  """
  # Number of characters
  n = 256 # len(data)//8

  # Create a new array to hold our character map image.
  # Make it 32 characters across by n/8 characters high.
  # Include an extra row and column pyplot.imshow clips the top and left sides.
  arr = np.ndarray(((n//32)*8 + 1, 32*8 + 1), dtype=np.uint8)

  # Loop over the "n" characters
  for i in range(0,n):
    x = 8*(i % 32) + 1
    y = 8*(i//32) + 1
    # Each byte is one line in one character
    for charline in range(0,8):
      dataIndex = i*8 + charline
      # Each bit is one dot in one line ordered from MSB to LSB
      for bit in range(0,8):
        arr[y + charline, x + 7 - bit] = (data[dataIndex] & 2**bit) != 0

  plt.figure(figsize=(15,5))
  plt.imshow(arr, cmap='Greys_r', interpolation='nearest',
    aspect=1/32, extent=[0,32,256,-1])
  plt.yticks([32,96,160,224], ['$00','$40','$80','$C0'], va='bottom', size=16)
  plt.xticks([0,8,16,24], ['$00','$08','$10','$18'], ha='center', size=16)
  plt.show()


def writeHeaderFile(inputFile, outputFile, data):
  """
  Convert the data in an Apple II ROM
  into a form suitable for an Arduino header file.
  """
  f = open(outputFile, 'w')
  f.write('// Use PROGMEM to put into Arduino flash memory\n')
  f.write('// ' + inputFile + ', created by applechargen.py\n')
  f.write(strftime("// %a, %d %b %Y %H:%M:%S\n", localtime()))
  f.write('// #define __PROG_TYPES_COMPAT__\n')
  f.write('// #include "avr/pgmspace.h"\n')
  f.write('#define ADDRESS_MAX ' + str(len(data)) + '\n')
  f.write('const unsigned char values[ADDRESS_MAX] PROGMEM = {\n')
  for i in range(0,len(data)//16):
    for j in range(0,16):
      f.write(str(data[i*16 + j]) + ",")
    f.write('\n')
  f.write('};\n')
  f.close()
  print('Output written to: ' + outputFile)

