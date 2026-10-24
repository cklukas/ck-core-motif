#!/usr/bin/env python3
"""Check that Mrm header RID allocation validates its input and output pointers."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__HDR_NextRID (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "if (file_id == NULL || res_id_return == NULL)"
if needle not in region:
    raise SystemExit("Mrm header RID allocation lacks null-input validation")
if region.index(needle) > region.index("resndx ="):
    raise SystemExit("Mrm header RID allocation checks pointers too late")
print("Mrm header RID allocation validates file and output pointers")
