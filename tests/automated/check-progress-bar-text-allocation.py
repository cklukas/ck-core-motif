#!/usr/bin/env python3
"""Check the progress-bar redisplay text allocation guard."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text(encoding="utf-8")
start = source.index("text_string = XmStringCreateLocalized(text);")
end = source.index("if (progress->progress", start)
region = source[start:end]

assert "if (text_string && progress->progress.font)" in region
assert "if (text_string)" in region
assert "XmStringExtent" in region
print("PASS progress-bar text allocation is guarded")
