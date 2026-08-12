out_file = "downscale.bin"

# Downscales from 16bit to 8bit.
# Assumes 5 bits of decimal, 3 bits headroom
def generateDownscaleAndSaturateTable():
    bytearr = bytearray([])
    n = 0
    for i in range(0,65536):
        if (i == 32768):
            n = -32768
        else:
            n += 1

        if (n >= 127 * 32):
            bytearr.append(127)
        elif (n <= -128 * 32):
            bytearr.append(128)
        elif (n >= 0):
            bytearr.append(n // 32)
        else:
            bytearr.append((n // 32) + 256)

    with open(out_file, "wb") as binary_file:
        binary_file.write(bytes(bytearr))
    

if __name__ == "__main__":
    generateDownscaleAndSaturateTable()
