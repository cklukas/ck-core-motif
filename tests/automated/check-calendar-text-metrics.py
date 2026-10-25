#!/usr/bin/env python3
"""Check Calendar labels use measured font baselines."""
from pathlib import Path

source = Path("lib/Xm/Calendar.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "text_baseline(calendar, 0, 28)" in source
assert "text_baseline(calendar, 28, 14)" in source
assert "text_baseline(calendar, top, 22)" in source
print("PASS Calendar text uses measured font baselines")
