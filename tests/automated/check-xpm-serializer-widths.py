#!/usr/bin/env python3
"""Check XPM serializer lengths are explicitly bounded at uint width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
assert "l = (unsigned int)(s - buf + 1);" in source
assert "sizeof(buf) <= (size_t)(s - buf)" in source
assert source.count("len > UINT_MAX || len > UINT_MAX - size") == 2
assert source.count("size += (unsigned int)len;") == 2
print("XPM serializer lengths use explicit uint-width bounds")
