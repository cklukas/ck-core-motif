#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
start = text.index("PropWMState *\nGetWMState")
body = text[start:text.index("/* END OF FUNCTION GetWMState", start)]
assert "actual_format == 32" in body
assert "nitems == PROP_WM_STATE_ELEMENTS" in body
print("GetWMState validates property format and length")
