#!/usr/bin/env python3
"""
Read the disassembly file and produce comments and breakpoint
for a mame debugger session.
"""

import sys
import re
from pathlib import Path

disasm_path = "out/rom.dis"
out_path = "debug/mame-session"

pattern_symbol = re.compile(r"([0-9a-f]{8,8}) <(.*)>:", flags=re.MULTILINE)

def main():
    disasm_file = open(disasm_path,'r')
    lines = disasm_file.readlines()
    disasm_file.close()

    bp_remain = sys.argv[1:]

    with open(out_path,'w') as out:
        for line in lines:
            matches = pattern_symbol.findall(line)
            for match in matches:
                addr = match[0]
                symbol = match[1]
                out.write(f"comadd {addr},{symbol}\n")
                if (bp_remain.count(symbol) != 0):
                    out.write(f"bp {addr}\n")
                    bp_remain.remove(symbol)

        # Start with breakpoints disabled
        out.write("\n")
        out.write("bpdisable\n")

    for remain in bp_remain:
        print(f"--- WARNING! Breakpoint symbol not found: {remain}")

if __name__ == "__main__":
    main()
