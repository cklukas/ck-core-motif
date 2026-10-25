#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
start = text.index("void \nGetDtWmRequest")
body = text[start:text.index("/* END OF FUNCTION GetDtWmRequest", start)]

validation = body.index("if ((rValue != Success)")
cleanup = body[validation:body.index("property = NULL", validation)]
assert "actualFormat != 8" in cleanup
assert "if (property != NULL)" in cleanup
assert "XFree ((char *)property)" in cleanup
print("GetDtWmRequest frees invalid property buffers before resetting state")
