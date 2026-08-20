import os
import math

script_dir = os.path.dirname(os.path.abspath(__file__))
out_file_s8_dec = os.path.join(script_dir, "mult_s8_dec.bin")
out_file_s8_int = os.path.join(script_dir, "mult_s8_int.bin")

def generateMultS8DecimalTable():
    bytearr = bytearray([])
    for m in range (0,256):
        m_float = m/256.0
        for n in range (0,256):
            # Make negative
            if (n >= 128):
                n -= 256

            # Mult and overflow
            v = math.trunc(n * m_float)
            if (v <= -128):
                b = 255
            elif (v < 0):
                b = v + 256
            elif (v > 127):
                b = 127
            else:
                b = v

            if (n == -1):
                print(f"m: {m}, n: {n}, mul: {n * m_float}, v: {v}, b: {b}")
            if (n == -10):
                print(f"m: {m}, n: {n}, mul: {n * m_float}, v: {v}, b: {b}")

            bytearr.append(b)

    with open(out_file_s8_dec, "wb") as binary_file:
                binary_file.write(bytes(bytearr))

def generateMultS8IntegerTable():
    bytearr = bytearray([])
    for m in range (0,256):
        for n in range (0,256):
            # Make negative
            if (n >= 128):
                n -= 256

            # Mult and clip/overflow
            v = n * m
            if (v <= -128):
                b = 255
            elif (v < 0):
                b = v + 256
            elif (v > 127):
                b = 127
            else:
                b = v
                 
            bytearr.append(b)

    with open(out_file_s8_int, "wb") as binary_file:
                binary_file.write(bytes(bytearr))

if __name__ == "__main__":
    generateMultS8DecimalTable()
    generateMultS8IntegerTable()
