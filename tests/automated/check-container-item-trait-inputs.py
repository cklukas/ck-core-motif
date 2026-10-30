#!/usr/bin/env python3
"""Check Container item-trait helpers validate widget and parent state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
for name in ("GetVisualEmphasis(\n", "SetVisualEmphasis(\n", "GetViewType(\n"):
    start = source.rindex(name)
    region = source[start : source.index("\n}\n", start)]
    assert "if (cwid == NULL)" in region
assert "if (cw == NULL)" in source[source.rindex("SetVisualEmphasis(\n") : source.index("\n}\n", source.rindex("SetVisualEmphasis(\n"))]

print("PASS Container item-trait helpers validate widget and parent state")
