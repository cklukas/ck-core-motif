#!/usr/bin/env python3
"""Check Badge text follows the widget render table."""
from pathlib import Path

source = Path("lib/Xm/Badge.c").read_text(encoding="utf-8")
assert "XmNrenderTable" in source
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmFontListCopy" in source
assert "XmRenderTableFree" in source
assert "if (new_font)" in source
assert "badge->badge.font = old->badge.font" in source
assert "badge->core.width > 1 && badge->core.height > 1" in source
print("PASS Badge text uses its render table")
