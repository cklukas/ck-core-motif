#!/usr/bin/env python3
"""Check vendor-shell WM hints validate the X property request status."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorSE.c").read_text()
start = source.index("GetMWMFunctionsFromProperty(")
end = source.index("/******* XmeFunctions", start)
body = source[start:end]

assert "result = XGetWindowProperty" in body
assert "(result != Success)" in body
assert "XFree( (char *)prop)" in body

print("Vendor-shell WM hints reject failed property requests")
