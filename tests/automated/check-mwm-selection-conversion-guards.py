#!/usr/bin/env python3
"""Check that MWM selection conversion initializes and validates outputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nWMiConvert (")
end = source.index("  if (!wmGD.Screens[scr].managed)", start)
region = source[start:end]

assert "w == NULL || outputType == NULL" in region
assert "return False;" in region
assert "*outputType = None;" in region
assert "*outputLen = 0;" in region
assert "*output    = NULL;" in region
print("PASS MWM selection conversion defaults are guarded")
