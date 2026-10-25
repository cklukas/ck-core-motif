#!/usr/bin/env python3
"""Check that XmScale selection conversion handles result allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text(encoding="utf-8")
start = source.index("DragConvertCallback (Widget w")
end = source.index("/************************************************************************\n *\n *  WidgetNavigable", start)
region = source[start:end]
assert "if (targs == NULL)" in region
assert region.count("cs->status = XmCONVERT_REFUSE;") >= 4
assert "if (value == NULL)" in region
assert "if (tp.value == NULL)" in region
assert "if (passtext == NULL)" in region
print("PASS XmScale conversion allocations are guarded")
