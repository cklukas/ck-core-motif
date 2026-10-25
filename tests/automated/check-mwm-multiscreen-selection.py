#!/usr/bin/env python3
"""Check that Mwm names each screen selection from that screen's number."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c").read_text(
    encoding="utf-8"
)
start = source.index("OwnWMSelections ( Time timestamp )")
region = source[start:source.index("#endif /* !defined(WSM)", start)]
assert '"WM_S%d"' in region
assert "wmGD.Screens[scr].screen" in region
assert "DefaultScreen(DISPLAY)" not in region
assert "snprintf(wm_scr, sizeof(wm_scr)" in region
print("Mwm per-screen selections use each managed screen number")
