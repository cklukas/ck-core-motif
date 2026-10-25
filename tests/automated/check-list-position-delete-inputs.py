#!/usr/bin/env python3
"""Check that position-based List deletion validates public inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nAPIDeletePositions")
end = source.index("XmListDeletePositions(Widget", start)
region = source[start:end]
assert "positions == NULL || count <= 0" in region
for function in ("XmListDeletePositions(", "XmListDeletePos(",
                 "XmListDeleteItemsPos("):
    start = source.rindex(function)
    body = source[start:source.index("\n}\n", start)]
    assert "if (w == NULL" in body
print("PASS position-based list deletion validates inputs")
