#!/usr/bin/env python3
"""Check that XIM callback registration validates its fixed slot index."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("regist_real_callback(Widget w")
end = source.index("static int\nNameToSwitch", start)
region = source[start:end]
assert "call == NULL" in region
assert "swc < 0 || swc >= 4" in region
assert "return;" in region
print("PASS XIM callback slot validation is guarded")
