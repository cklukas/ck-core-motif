#!/usr/bin/env python3
"""Check that cascade-arrow pixmap and GC resources are validated."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c"
text = source.read_text()
start = text.index("_XmCreateArrowPixmaps(")
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

print("cascade-arrow pixmap and GC creation is guarded")
