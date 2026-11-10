#!/usr/bin/env python3
"""Check Editres 32-bit decoding uses unsigned-wide arithmetic and that signed
16-bit values keep their sign."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nWireGet32(')
end = source.index('\n/*\tFunction Name: WireGetBytes', start)
region = source[start:end]

assert "((uint32_t)hi << 16) | (uint32_t)lo" in region
assert "((unsigned short) temp1 << (XER_NBBY * 2))" not in source

start = source.index("\nWireGetSigned16(")
end = source.index("\n/*\tFunction Name: WireGet32", start)
region = source[start:end]
assert "(short)((int)raw - 0x10000)" in region
assert "&= (temp1 << XER_NBBY)" not in source

print("Editres 32-bit decode uses unsigned-wide arithmetic")
