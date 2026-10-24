#!/usr/bin/env python3
"""Check XTextProperty selection export copies."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c"
text = source.read_text()
assert text.count("(unsigned long)(size_t)tmp_prop.nitems") == 3
assert text.count("tmp_prop.nitems ?") == 3
assert text.count("if (tmp_prop.nitems != 0)") == 3
print("text selection export bounds contract passed")
