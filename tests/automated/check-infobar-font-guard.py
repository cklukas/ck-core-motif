#!/usr/bin/env python3
"""Ensure XmInfoBar installs a drawable GC font instead of silently skipping text."""
from pathlib import Path

root = Path(__file__).parents[2]
part = (root / "lib/Xm/InfoBarP.h").read_text(encoding="utf-8")
source = (root / "lib/Xm/InfoBar.c").read_text(encoding="utf-8")
assert "XFontStruct *font" in part
assert 'XLoadQueryFont(display,"fixed")' in source
assert "GCFont" in source
assert "font = bar->info.font;" in source
assert "XGetGCValues" not in source
print("PASS InfoBar text GC has an installed fallback font")
