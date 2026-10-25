#!/usr/bin/env python3
"""Check Sash focus painting cannot wrap on undersized widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sash.c").read_text(encoding="utf-8")
for name in ("HighlightSash", "UnhighlightSash"):
    start = source.index("\n" + name + "(\n")
    start = source.index("{", start)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "sash->core.width <= 2 * x" in region
    assert "sash->core.height <= 2 * y" in region
    assert "return;" in region
assert source.count("sash->core.width <= 2 * x") >= 2
print("sash highlight geometry is bounded")
