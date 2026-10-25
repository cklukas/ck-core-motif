#!/usr/bin/env python3
"""Check ToggleButton visual-state defaults return without asserting."""

from pathlib import Path


for name in ("lib/Xm/ToggleB.c", "lib/Xm/ToggleBG.c"):
    source = Path(name).read_text()
    assert "case XmINDETERMINATE:" in source
    assert "assert(False);" not in source

print("ToggleButton invalid visual states fall back safely")
