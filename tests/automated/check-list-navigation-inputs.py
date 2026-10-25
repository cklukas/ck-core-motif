#!/usr/bin/env python3
"""Check that List selection/navigation APIs validate widget inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
for function in ("XmListDeselectAllItems(", "XmListSetPos(",
                 "XmListSetBottomPos("):
    start = source.rindex(function)
    body = source[start:source.index("\n}\n", start)]
    assert "if (w == NULL)" in body
    assert body.index("_XmWidgetToAppContext(w)") > body.index("if (w == NULL)")
print("PASS list selection/navigation APIs validate widget inputs")
