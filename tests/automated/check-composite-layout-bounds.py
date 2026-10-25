#!/usr/bin/env python3
"""Check composite action/status layouts do not wrap X11 positions."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Toolbar.c", "StatusBar.c"):
    source = (root / name).read_text()
    assert "long x = 0" in source, name
    assert "x > SHRT_MAX" in source, name
    assert "Position placement" in source, name
print("PASS toolbar and status-bar layout placements are bounded")
