#!/usr/bin/env python3
"""Check MWM screen-name allocation keeps size_t arithmetic before XtRealloc."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text()
start = source.index("InitWmScreen (WmScreenData *pSD, int sNum)")
region = source[start:source.index("void\nInitWmDisplayEnv", start)]

assert "size_t buf_size;" in region
assert "buf_size = strlen(buffer) + 1;" in region
assert "if (buf_size > UINT_MAX)" in region
assert "XtRealloc (wmGD.screenNames[sNum], (Cardinal)buf_size)" in region
assert region.index("if (buf_size > UINT_MAX)") < region.index("XtRealloc")
print("PASS MWM screen-name allocation bounds its Cardinal conversion")
