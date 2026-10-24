#!/usr/bin/env python3
"""Check bounded ColorObj atom-name formatting."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")
assert '(void)snprintf(names[i], MAX_ATOM_LENGTH' in text
assert 'sprintf(names[i], "%s%d"' not in text
print("ColorObj atom names are bounded")
