#!/usr/bin/env python3
"""Check Spinner initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text()
start = source.index("static void Initialize(Widget request")
end = source.index("\nstatic void Destroy", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = XmStringCopy(spinner->spinner.label);" in region
assert "replacement != NULL" in region
assert "spinner->spinner.label = XmStringCreateLocalized(\"\");" in region
assert "if (spinner->spinner.font && spinner->spinner.label)" in region
print("Spinner initialization handles label allocation failure")
