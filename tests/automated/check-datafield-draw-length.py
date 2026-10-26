#!/usr/bin/env python3
"""Check DataField Unicode drawing uses checked size arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.rindex("df_DrawText(\n        XmDataFieldWidget tf")
end = source.index("\nstatic ", start)
body = source[start:end]
assert body.count("length < 0") >= 2
assert body.count("byte_capacity") >= 4
assert body.count("wcstombs(tmp, wc_string, byte_capacity)") == 2

print("DataField Unicode drawing uses checked size arithmetic")
