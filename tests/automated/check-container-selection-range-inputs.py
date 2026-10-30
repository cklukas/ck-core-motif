#!/usr/bin/env python3
"""Check Container selection-range helpers validate widget inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
for name in ("ResetMarkedCwids(\n", "MarkCwidsInRange(\n", "MarkCwidsInMarquee(\n"):
    start = source.rindex(name)
    region = source[start : source.index("\n}\n", start)]
    assert "if (wid == NULL)" in region
    assert "return(False);" in region

print("PASS Container selection-range helpers validate widget inputs")
