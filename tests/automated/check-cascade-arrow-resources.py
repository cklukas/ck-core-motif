#!/usr/bin/env python3
"""Check that cascade-arrow pixmap and GC resources are validated."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c"
text = source.read_text()
start = text.index("\n_XmCreateArrowPixmaps(")
end = text.index("\n}\n", start) + 3
block = text[start:end]

for needle in (
    "if (unarmed_arrow == NULL || armed_arrow == NULL)",
    "if (gc == NULL)",
    "if (pixmap != None)",
    "Boolean arm_gc_owned = False;",
):
    if needle not in block:
        raise SystemExit(f"cascade-arrow setup is missing {needle}")

assert block.index("if (unarmed_arrow == NULL || armed_arrow == NULL)") < block.index("unarmed_arrow->pixmap")
assert block.index("if (gc == NULL)") < block.index("XFillRectangle(")
assert block.count("if (pixmap != None)") == block.count("pixmap = XCreatePixmap(") == 2

print("cascade-arrow pixmap and GC creation is guarded")
