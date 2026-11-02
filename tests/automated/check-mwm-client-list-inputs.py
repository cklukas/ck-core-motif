#!/usr/bin/env python3
"""Check MWM client-list inputs are safe for the Xlib count parameter."""

from pathlib import Path


source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
start = source.index("void SetMwmClientList")
body = source[start : source.index("} /* END OF FUNCTION SetMwmClientList */", start)]

assert "wmWindowCount != 0" in body
assert "wmWindowList == NULL" in body
assert "wmWindowCount > (unsigned int)INT_MAX" in body
assert "return;" in body
assert body.index("if (wmWindowCount > (unsigned int)INT_MAX") < body.index(
    "XChangeProperty"
)

print("PASS MWM client-list setter rejects invalid storage and oversized counts")
