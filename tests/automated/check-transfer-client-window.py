#!/usr/bin/env python3
"""Check client-window conversion handles a widget without a shell."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
start = source.index("} else if (atoms[XmACLIENT_WINDOW] == cs -> target)")
region = source[start:source.index("} else if (atoms[XmA_MOTIF_RENDER_TABLE]", start)]
assert "if (current == NULL)" in region
assert "XtFree((char *)cw);" in region
assert region.index("if (current == NULL)") < region.index("*cw = XtWindow(current);")
print("PASS client-window conversion handles missing shell ancestry")
