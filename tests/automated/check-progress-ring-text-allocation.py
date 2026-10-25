#!/usr/bin/env python3
"""Check the progress-ring redisplay text allocation guard."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressRing.c").read_text(encoding="utf-8")
start = source.index("text_string = XmStringCreateLocalized(text);")
end = source.index("if (ring->progress_ring.label", start)
region = source[start:end]

assert "if (text_string)" in region
assert "XmStringExtent" in region
assert "XmStringFree(text_string);" in region
print("PASS progress-ring text allocation is guarded")
