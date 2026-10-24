#!/usr/bin/env python3
"""Check bounded generated XmScreen widget names."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(encoding="utf-8")
assert "char name[25];" in text
assert 'snprintf(name, sizeof(name), "screen%d", i)' in text
assert 'sprintf(name, "screen%d", i)' not in text
print("XmScreen generated names are bounded")
