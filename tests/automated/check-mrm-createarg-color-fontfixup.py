#!/usr/bin/env python3
"""Check Mrm color and font reader boundaries reject null records."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_CreateArglist (")
end = text.find("\n\n\f", start)
region = text[start:end]
if "if ( colorptr == NULL )" not in region:
    raise SystemExit("CreateArglist color guard is missing")
start = text.index("\nUrm__CW_FixupValue (")
end = text.find("\n\n\f", start)
region = text[start:end]
if "if ( fontitem == NULL || bufptr == NULL )" not in region:
    raise SystemExit("font fixup guard is missing")
print("PASS Mrm color and font reader boundaries reject null records")
