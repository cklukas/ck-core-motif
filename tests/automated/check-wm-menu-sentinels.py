#!/usr/bin/env python3
"""Check Mwm menu sentinel arguments use type-correct zero constants."""

from pathlib import Path


source = Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c"
text = source.read_text(encoding="utf-8")
region = text[text.index("static MenuItem *MakeClientCommandMenuItem"):
              text.index("PerformClientCommandMatch", text.index("static MenuItem *MakeClientCommandMenuItem"))]
assert "(KeySym)0" in region
assert "(KeyCode)0" in region
assert "(KeySym)(unsigned)NULL" not in region
assert "(KeyCode)(unsigned)NULL" not in region
assert "tree == NULL ? (CARD32)0" in text
assert "tree == NULL ? (CARD32)NULL" not in text
print("WmMenu sentinel values are type-correct")
