#!/usr/bin/env python3
"""Check MWM button bindings pass the callback's declared XEvent pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("Boolean CheckForButtonAction")
end = source.index("END OF FUNCTION CheckForButtonAction", start)
body = source[start:end]

assert "buttonSpec->wmFunction (buttonSpec->wmFuncArgs, pCD," in body
assert "(XEvent *)buttonEvent" in body
assert "buttonSpec->wmFunction (buttonSpec->wmFuncArgs, pCD,\n\t\t\t\t\t      buttonEvent)" not in body
print("PASS MWM button bindings use the declared XEvent callback type")
