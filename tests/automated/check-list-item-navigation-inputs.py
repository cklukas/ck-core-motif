#!/usr/bin/env python3
"""Check that item-based List navigation validates public inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
for function in ("XmListSetItem(", "XmListSetBottomItem("):
    start = source.rindex(function)
    body = source[start:source.index("\n}\n", start)]
    assert "if (w == NULL || item == NULL)" in body
    assert body.index("_XmWidgetToAppContext(w)") > body.index("if (w == NULL")
    assert body.index("ItemNumber(lw, item)") > body.index("if (w == NULL")
print("PASS item-based list navigation validates inputs")
