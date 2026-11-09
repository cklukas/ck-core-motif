#!/usr/bin/env python3
"""Check the remaining modern composite controls use Xt's reply helper."""

from pathlib import Path
import re


root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Disclosure.c", "EmptyState.c", "Flyout.c", "FormSection.c",
             "RowActions.c", "Toast.c"):
    source = (root / name).read_text(encoding="utf-8")
    match = re.search(
        r"static XtGeometryResult QueryGeometry\(Widget widget.*?\{(?P<body>.*?)\}",
        source,
        re.DOTALL,
    )
    assert match, name
    query = match.group("body")
    assert "XmeReplyToQueryGeometry(widget," in query, name
    assert "return XtGeometryAlmost;" not in query, name

print("Remaining composite QueryGeometry methods honor Xt request-mode semantics")
