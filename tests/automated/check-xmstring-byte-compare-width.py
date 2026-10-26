#!/usr/bin/env python3
"""Check that XmStringByteCompare preserves the converter's length width."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringObso.c"
text = source.read_text(encoding="utf-8")
start = text.index("XmStringByteCompare(")
end = text.index("\n}\n", start)
region = text[start:end]

if "unsigned int a_length, b_length;" not in region:
    raise SystemExit("XmStringByteCompare still narrows byte-stream lengths")
if "unsigned short a_length" in region or "unsigned short b_length" in region:
    raise SystemExit("XmStringByteCompare retains a 16-bit length")

print("XmStringByteCompare preserves full byte-stream length")
