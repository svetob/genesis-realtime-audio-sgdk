#!/usr/bin/env python3
"""
Convert GCC's AT&T ASM syntax to Motorola 68K syntax.

"""

import sys
from pathlib import Path


def main():
    scan_dir = Path(sys.argv[1])

    for lst in sorted(scan_dir.rglob("*.lst")):
        text = lst.read_text(encoding="utf-8", errors="surrogateescape")
        text_m68 = text.replace("%", "")
        dest = lst.with_suffix(".s")
        dest.write_text(text_m68, encoding="utf-8", errors="surrogateescape")
        print(f"Converted {lst} to M68k ASM: {dest}")

    return 0


if __name__ == "__main__":
    main()
