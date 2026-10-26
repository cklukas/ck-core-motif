#!/usr/bin/env python3
"""Check Link text and underline geometry use active font metrics."""
from pathlib import Path

source = Path("lib/Xm/Link.c").read_text(encoding="utf-8")
assert "XmStringExtent(link->link.render_table, label" in source
assert "int underline_end = 4 + (int)text_width" in source
assert "link->core.width > 4 && link->core.height >= 5" in source
assert "if (new_table)" in source
assert "link->link.render_table = old->link.render_table" in source
print("PASS Link text and underline geometry are guarded")
