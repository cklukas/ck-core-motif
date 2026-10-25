#!/usr/bin/env python3
"""Check Mrm value conversion validates vector metadata before repacking."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_ConvertValue (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
if region.count("vecptr == NULL || vecptr->validation != URMTextVectorValid") != 2:
    raise SystemExit("text-vector conversion guards are incomplete")
if "vecptr == NULL || vecptr->validation != URMIntegerVectorValid" not in region:
    raise SystemExit("integer-vector conversion guard is missing")
if region.count("vecptr->count > RGMListSizeMax") < 3:
    raise SystemExit("vector count limits are incomplete")
print("PASS Mrm value conversion validates vector metadata")
