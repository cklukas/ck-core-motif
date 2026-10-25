#!/usr/bin/env python3
"""Check text line scanning treats Look's result as a pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("case XmSELECT_LINE:")
end = source.index("\n  case ", start)
segment = source[start:end]

assert "c = Look(data, position, dir);" in segment
assert "(c == NULL) || (*c == *data->PSWC_NWLN)" in segment
assert "(c == '\\0')" not in segment
print("PASS text line Look pointer guard")
