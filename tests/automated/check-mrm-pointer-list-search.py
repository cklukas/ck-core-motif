#!/usr/bin/env python3
"""Check Mrm pointer-list string search skips null slots."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmPlistFindString (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
if "UrmPlistPtrN(list_id,ndx) != NULL" not in region:
    raise SystemExit("Mrm string search does not guard null pointer slots")
print("Mrm pointer-list string search skips null slots safely")
