#!/usr/bin/env python3
"""Check icon lookup does not copy HOME into a fixed path buffer."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c"
text = source.read_text()
assert "strcpy(stackString, homedir)" not in text
print("icon path fixed-buffer contract passed")
