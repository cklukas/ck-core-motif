#!/usr/bin/env python3
"""Check appearance-profile numeric conversion rejects strtoul overflow."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Appearance.c").read_text()
assert "number = strtoul(text, &end, 10);" in source
assert "if (errno == ERANGE || *text == '\\0'" in source
assert "number > (unsigned long)~(Dimension)0" in source
print("PASS appearance-profile numbers reject conversion overflow")
