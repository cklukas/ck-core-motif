#!/usr/bin/env python3
"""Check Mrm writer child index input is validated."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetChild (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
check = region.find("if ( index == NULL || strlen(index) <= 0 )")
if check < 0:
    raise SystemExit("indexed-child path lacks null-input validation")
strlen = region.find("strlen(index)", check)
if strlen < check:
    raise SystemExit("child index is not checked before string access")

print("Mrm writer child index input is validated")
