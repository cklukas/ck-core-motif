#!/usr/bin/env python3
"""Check icon-box resize validates state before division or dereference."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

needle = "void CheckIconBoxResize (ClientData *pCD"
start = text.index(needle)
guard = text[start:text.index("pIPD =", start)]
assert "pCD == NULL" in guard
assert "thisIconBox == NULL" in guard
assert "widthInc <= 0 || pCD->heightInc <= 0" in guard
assert "return;" in guard
print("icon-box resize validates state and increments")
