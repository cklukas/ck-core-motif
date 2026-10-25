#!/usr/bin/env python3
"""Check that F_Screen maps a single managed screen to array index zero."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmFunction.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean F_Screen")
region = source[start:source.index("if (strcmp (args, \"next\")", start)]
assert "(wmGD.numScreens == 1) ? 0 : DefaultScreen(DISPLAY)" in region
print("F_Screen handles nonzero default screen numbers safely")
