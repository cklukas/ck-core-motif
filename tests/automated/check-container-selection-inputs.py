#!/usr/bin/env python3
"""Check Container selection helpers validate child and constraint inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
for name, signature, dereference in (("MarkCwid(", "MarkCwid(\n    Widget", "XtIsSensitive(cwid)"), ("UnmarkCwidVisual(", "UnmarkCwidVisual(\n        Widget", "c->selection_visual")):
    start = source.index(signature)
    region = source[start:source.index("\n}", start)]
    assert "if (cwid == NULL)" in region, name
    assert "if (cw == NULL || c == NULL)" in region, name
    assert region.index("if (cw == NULL || c == NULL)") < region.index(dereference), name
print("PASS Container selection helpers validate child and constraint inputs")
