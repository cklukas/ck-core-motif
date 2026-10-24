#!/usr/bin/env python3
"""Check that the PNG loader rejects truncated signatures before parsing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Png.c").read_text()
assert "if (fread(sig, 1, sizeof(sig), infile) != sizeof(sig))" in source
assert source.index("fread(sig, 1, sizeof(sig), infile)") < source.index("png_sig_cmp(sig, 0, 8)")
assert "fread(sig, 1, 8, infile);" not in source
print("PNG loader checks the complete signature")
