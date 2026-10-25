#!/usr/bin/env python3
"""Check unnormalized ToggleButton indicators do not assert."""

from pathlib import Path


for name in ("lib/Xm/ToggleB.c", "lib/Xm/ToggleBG.c"):
    source = Path(name).read_text()
    start = source.index("case XmONE_OF_MANY:")
    region = source[start:source.index("case XmONE_OF_MANY_DIAMOND:", start)]
    assert "break;" in region
    assert "assert(FALSE);" not in region

print("ToggleButton unnormalized indicators fall back safely")
