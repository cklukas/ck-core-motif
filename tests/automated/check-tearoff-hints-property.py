#!/usr/bin/env python3
"""Check tear-off WM hints require a complete property."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TearOff.c").read_text()
start = source.index("_XmTearOffInitiate(")
end = source.index("/* Notify the server of the change */", start)
body = source[start:end]

assert "property_result = XGetWindowProperty" in body
assert "property_result != Success" in body
assert "num_items < PROP_MWM_HINTS_ELEMENTS" in body
assert "num_items < PROP_MOTIF_WM_INFO_ELEMENTS" not in body

print("Tear-off WM hints require a complete property")
