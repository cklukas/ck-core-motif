#!/usr/bin/env python3
"""Check that MWM does not retain a truncated SM client identifier."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWinInfo.c").read_text(
    encoding="utf-8"
)
start = source.index("void \nProcessSmClientID")
end = source.index("} /* END OF FUNCTION ProcessSmClientID */", start)
body = source[start:end]

assert "(leftover == 0)" in body
assert body.index("(leftover == 0)") < body.index("XtMalloc((Cardinal)nitems + 1)")
assert "XFree(clientID);" in body

print("PASS MWM session client IDs require a complete property read")
