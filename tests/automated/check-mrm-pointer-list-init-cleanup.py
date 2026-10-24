#!/usr/bin/env python3
"""Check Mrm pointer-list initialization cleans up partial allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrmPlistInit (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
if "XtFree ((char *)*list_id_return)" not in region:
    raise SystemExit("Mrm pointer-list init does not free its partial list")
if "*list_id_return = NULL" not in region:
    raise SystemExit("Mrm pointer-list init leaves a dangling output pointer")
print("Mrm pointer-list initialization cleans up vector allocation failure")
