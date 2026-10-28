#!/usr/bin/env python3
"""Check Editres 32-bit decoding uses unsigned-wide arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index("_XEditResGet32(stream, val)")
end = source.index("_XEditResGetString8(stream, str)", start)
region = source[start:end]

assert "((unsigned long)temp1 << (XER_NBBY * 2))" in region
assert "(unsigned long)temp2" in region
assert "((unsigned short) temp1 << (XER_NBBY * 2))" not in region

print("Editres 32-bit decode uses unsigned-wide arithmetic")
