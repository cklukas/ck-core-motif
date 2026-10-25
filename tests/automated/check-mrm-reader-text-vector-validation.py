#!/usr/bin/env python3
"""Check reader text-vector metadata before iterating entries."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_FixupValue (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
if "if ( vecptr == NULL )" not in region:
    raise SystemExit("text-vector null guard is missing")
if "vecptr->validation != URMTextVectorValid" not in region:
    raise SystemExit("text-vector validation marker is not checked")
if "vecptr->count > RGMListSizeMax" not in region:
    raise SystemExit("text-vector count limit is not checked")
print("PASS Mrm reader text-vector metadata is validated")
