out_file = "mixclip8b.bin"

# Mixes two unsigned 8bit samples together, outputs a signed 8bit sample
# A = 0-255, 128 center
# B = 0-255, 128 center
# M = A + B = 0-511, 256 center
def generate8bitMixAndSaturateTable():
    bytearr = bytearray([])
    for m in range(0,512):
        if (m <= 128):
            bytearr.append(128)
        elif (m < 256):
            bytearr.append(m)
        elif (m < 384):
            bytearr.append(m - 256)
        else:
            bytearr.append(127)

    with open(out_file, "wb") as binary_file:
            binary_file.write(bytes(bytearr))

if __name__ == "__main__":
    generate8bitMixAndSaturateTable()
