#!/usr/bin/env python3
"""Check Mrm writer class input is validated before string operations."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetClass (URMResourceContextPtr")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
check = region.find("if ( class == NULL || strlen(class) <= 0 )")
if check < 0:
    raise SystemExit("unknown-class path lacks null-input validation")
strlen = region.find("strlen(class)", check)
if strlen < check:
    raise SystemExit("class is not checked before string access")

print("Mrm writer user-defined class input is validated")
