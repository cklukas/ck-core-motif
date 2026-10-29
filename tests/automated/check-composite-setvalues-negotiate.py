#!/usr/bin/env python3
"""Check composite spacing changes renegotiate preferred geometry."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name, marker in (("Toolbar.c", "spacing_changed"),
                     ("StatusBar.c", "old->status_bar.spacing")):
    source = (root / name).read_text()
    start = source.index("static Boolean SetValues(Widget old_widget")
    end = source.index("static XtGeometryResult GeometryManager", start)
    body = source[start:end]
    assert marker in body, name
    assert "negotiate(" in body, name
    assert "layout(" not in body.split("negotiate(", 1)[0], name
print("PASS composite spacing changes renegotiate geometry")
