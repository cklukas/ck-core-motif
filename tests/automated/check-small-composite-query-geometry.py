#!/usr/bin/env python3
"""Check small composite controls honor Xt geometry request modes."""

from pathlib import Path
import re


root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("DropHighlight.c", "Skeleton.c"):
    source = (root / name).read_text(encoding="utf-8")
    match = re.search(
        r"static XtGeometryResult QueryGeometry\(Widget widget.*?\{(?P<body>.*?)\}",
        source,
        re.DOTALL,
    )
    assert match, name
    query = match.group("body")
    assert "XmeReplyToQueryGeometry(widget," in query, name
    assert "return XtGeometryYes;" not in query, name

drop_highlight = (root / "DropHighlight.c").read_text(encoding="utf-8")
assert "static Dimension bounded_dimension(size_t value)" in drop_highlight
assert "bounded_dimension((size_t)label_width +" in drop_highlight
assert "bounded_dimension((size_t)label_height +" in drop_highlight

print("DropHighlight and Skeleton honor Xt geometry request-mode semantics")
