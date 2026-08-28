import os

script_dir = os.path.dirname(os.path.abspath(__file__))
out_file_u8 = os.path.join(script_dir, "mixclip_u8.bin")
out_file_s8 = os.path.join(script_dir, "mixclip_s8.bin")
out_file_s8_xl = os.path.join(script_dir, "mixclip_s8_xl.bin")

# Mixes two unsigned 8bit samples together, outputs a signed 8bit sample
# A = 0-255, 128 center
# B = 0-255, 128 center
# M = A + B = 0-511, 256 center
# OUT = -128-127, 0 center
def generate8bitMixAndSaturateTable_S8():
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

    with open(out_file_s8, "wb") as binary_file:
            binary_file.write(bytes(bytearr))

# Mixes two unsigned 8bit samples together, outputs a signed 8bit sample
# M = A + B = 0-32768, 16384 center
# OUT = -128-127, 0 center
def generate8bitMixAndSaturateTable_S8_XL():
    bytearr = bytearray([])
    for m in range(0,32768):
        if (m <= 16384-128):
            bytearr.append(128)
        elif (m < 16384):
            bytearr.append((m - 16384) + 256)
        elif (m < 16384 + 128):
            bytearr.append(m - 16384)
        else:
            bytearr.append(127)

    with open(out_file_s8_xl, "wb") as binary_file:
            binary_file.write(bytes(bytearr))


# Mixes two unsigned 8bit samples together, outputs an unsigned 8bit sample
# A = 0-255, 128 center
# B = 0-255, 128 center
# M = A + B = 0-511, 256 center
# OUT = 0-255, 128 center
def generate8bitMixAndSaturateTable_U8():
    bytearr = bytearray([])
    for m in range(0,512):
        if (m <= 128):
            bytearr.append(0)
        elif (m < 384):
            bytearr.append(m - 128)
        else:
            bytearr.append(255)

    with open(out_file_u8, "wb") as binary_file:
            binary_file.write(bytes(bytearr))

if __name__ == "__main__":
    generate8bitMixAndSaturateTable_S8()
    generate8bitMixAndSaturateTable_S8_XL()
    generate8bitMixAndSaturateTable_U8()
