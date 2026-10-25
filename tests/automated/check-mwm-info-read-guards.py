#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
start = text.index("PropMwmInfo *GetMwmInfo")
body = text[start:text.index("/* END OF FUNCTION GetMwmInfo", start)]
assert "actual_format == 32" in body
assert "nitems == PROP_MWM_INFO_ELEMENTS" in body
print("GetMwmInfo validates property format and length")
