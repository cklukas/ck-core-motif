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

print("DropHighlight and Skeleton honor Xt geometry request-mode semantics")
