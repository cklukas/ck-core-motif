#!/usr/bin/env python3
"""Check VendorShell direction/unit traits handle incomplete state."""

from pathlib import Path


source = Path("lib/Xm/VendorS.c").read_text(encoding="utf-8")
start = source.rindex("GetDirection(Widget w)")
end = source.index("\n}", start)
direction = source[start:end]
assert "if (w == NULL)" in direction
assert "extData == NULL || extData->widget == NULL" in direction
assert "return XmRIGHT_TO_LEFT" in direction

start = source.rindex("GetUnitType(Widget w)")
end = source.index("\n}", start)
unit = source[start:end]
assert "if (w == NULL)" in unit
assert "return XmPIXELS" in unit

print("VendorShell trait guards present")
