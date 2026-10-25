#!/usr/bin/env python3
"""Check bounded ColorObj property serialization."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")
start = text.index("names = ((size_t)nscreens")
end = text.index("XmStackFree((char *)names", start)
region = text[start:end]
assert region.count("snprintf(names[i], MAX_ATOM_LENGTH") == 1
assert "sprintf(" not in region
print("ColorObj property serialization is bounded")
