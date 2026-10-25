#!/usr/bin/env python3
"""Check ProgressBar initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text()
start = source.index("static void Initialize(Widget request")
end = source.index("\nstatic void Destroy", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = XmStringCopy(progress->progress.label);" in region
assert "replacement != NULL" in region
assert "progress->progress.label = XmStringCreateLocalized(\"\");" in region
assert "if (progress->progress.font && progress->progress.label)" in region
print("ProgressBar initialization handles label allocation failure")
