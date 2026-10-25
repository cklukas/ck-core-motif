#!/usr/bin/env python3
"""Check MWM identity detection copies the executable basename."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMain.c").read_text(
    encoding="utf-8"
)
start = source.index("int WmReturnIdentity")
body = source[start : source.index("} /* END OF FUNCTION WmReturnIdentity", start)]
assert "baseName = strrchr(argv[0], '/');" in body
assert "baseName++;" in body
assert "executableLength >= UINT_MAX" in body
assert "XtMalloc((Cardinal)executableLength + 1)" in body
assert "XtMalloc ((unsigned int)(strlen (argv[0]) + 1))" not in body
assert "strcpy(tempString, baseName);" in body
assert "tempString++;" not in body

print("MWM identity detection uses the executable basename")
