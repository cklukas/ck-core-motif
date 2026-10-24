#!/usr/bin/env python3
"""Check Mrm writer comment input is validated before string operations."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("Cardinal UrmCWRSetComment (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
check = region.find("if (comment == NULL)")
if check < 0:
    raise SystemExit("UrmCWRSetComment lacks null-input validation")
strlen = region.find("strlen(comment)", check)
if strlen < 0 or strlen < check:
    raise SystemExit("comment is never checked before string access")

print("Mrm writer comment input is validated")
