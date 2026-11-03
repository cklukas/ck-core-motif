#!/usr/bin/env python3
"""Check modern composite controls use Xt's request-mode-aware reply helper."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Sidebar.c", "Toolbar.c", "StatusBar.c", "BreadcrumbBar.c",
             "CommandPalette.c"):
    source = (root / name).read_text(encoding="utf-8")
    start = source.index("static XtGeometryResult QueryGeometry(Widget widget")
    end = source.find("\n}\n", start) + 2
    query = source[start:end]
    assert "reply->request_mode = CWWidth | CWHeight;" in query, name
    assert "XmeReplyToQueryGeometry(widget," in query, name
    assert "return XtGeometryAlmost;" not in query, name

print("Composite QueryGeometry methods honor Xt request-mode semantics")
