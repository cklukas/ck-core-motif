#!/usr/bin/env python3
"""Check Mrm related argument codes are restricted to vector values."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
marker = "case MrmRtypeIntegerVector:"
pos = region.index(marker)
block = region[pos:region.find("\n\t      }", pos)]
if "default:" not in block or "A related code is a vector-count resource by definition." not in block:
    raise SystemExit("related-code type guard is missing")
print("PASS Mrm related codes are restricted to vector values")
