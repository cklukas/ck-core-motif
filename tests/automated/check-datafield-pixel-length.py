#!/usr/bin/env python3
"""Check DataField pixel measurement uses checked Unicode sizes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("\ndf_FindPixelLength( tf, string, length)")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
assert body.count("length < 0") == 2
assert body.count("byte_capacity") >= 4
assert body.count("wcstombs(tmp, wc_string, byte_capacity)") == 2

print("DataField pixel measurement uses checked Unicode sizes")
