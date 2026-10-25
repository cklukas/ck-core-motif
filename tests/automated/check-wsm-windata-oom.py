#!/usr/bin/env python3
"""Check shared WSM window-data unpacking fails closed on missing state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("UnpackWinData(MessageData", text.index("Function Name: UnpackWinData"))
end = text.index("\n}\n", start) + 2
region = text[start:end]

assert "*win_data = NULL;" in region
assert "*num = 0;" in region
assert "if (conf_fmt == NULL" in region
assert "if (bit_mask == NULL)" in region
assert "if (ptr == NULL)" in region
assert "XtFree((XtPointer) bit_mask);" in region
print("WSM window-data unpacking handles missing and failed allocations")
