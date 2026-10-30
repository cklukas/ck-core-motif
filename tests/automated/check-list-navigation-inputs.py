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
    guard = "!XmIsList(w)" if function == "XmListDeselectAllItems(" else "if (w == NULL)"
    assert guard in body
    assert body.index("_XmWidgetToAppContext(w)") > body.index(guard)
print("PASS list selection/navigation APIs validate widget inputs")
