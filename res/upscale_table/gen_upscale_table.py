out_file = "upscale.bin"

# Upscales from 8bit to 16bit.
# Assumes 5 bits of decimal, 3 bits headroom
def generateUpscaleTable():
    bytearr = bytearray([])
    for i in range(0,256):
        word = i * 32
        if (i > 127):
            word = word + (65536 - (256 * 32))


        hi = word // 256
        lo = word % 256
        print (i, hex(word), hex(hi), hex(lo))

        bytearr.append(hi)
        bytearr.append(lo)

    with open(out_file, "wb") as binary_file:
        binary_file.write(bytes(bytearr))
    

if __name__ == "__main__":
    generateUpscaleTable()
