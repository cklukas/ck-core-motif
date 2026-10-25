#!/usr/bin/env python3
"""Check List geometry/query APIs before toolkit access or indexing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.rindex("XmListSetHorizPos(")
body = source[start:source.index("\n}\n", start)]
assert "if (w == NULL)" in body
assert body.index("_XmWidgetToAppContext(w)") > body.index("if (w == NULL)")
start = source.rindex("XmListPosSelected(")
body = source[start:source.index("\n}\n", start)]
assert "lw->list.itemCount <= 0" in body
assert "lw->list.LastItem <= 0" in body
print("PASS list query/navigation inputs are guarded")
