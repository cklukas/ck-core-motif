#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
start = text.index("PropMwmHints *\nGetMwmHints")
body = text[start:text.index("/* END OF FUNCTION GetMwmHints", start)]
assert "actual_format == 32" in body
assert "nitems >= PROP_MWM_HINTS_ELEMENTS" in body
print("GetMwmHints validates property format and length")
