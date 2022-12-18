from sys import argv

# open file
file = open(argv[1], 'rb')
buffer = bytearray(file.read())
file.close()

# determine padding amount
pad_count = len(buffer) % 32
if pad_count:
  print('padding', pad_count)
  buffer.extend(bytes(32 - pad_count))

file = open(argv[1], 'wb')
file.write(buffer)
file.close()