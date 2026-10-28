#!/usr/bin/env python3
"""Check that Motif unit parsing classifies input bytes safely."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ResInd.c").read_text()
start = source.index("ParseUnitString(")
end = source.index("\n}\n\n\n\n/**********************************************************************", start)
region = source[start:end]

assert "unsigned char c;" in region
assert region.count("isascii(c=(unsigned char)*string)") == 2
assert "isspace(c)" in region
assert "isascii(c=*string)" not in region
print("unit-string byte classification contract passed")
