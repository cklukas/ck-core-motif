#!/usr/bin/env python3
"""Ensure Mrm text-vector fixup validates pointers and signed counts."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_FixupValue (long")
end = text.index("\n}\n", start)
body = text[start:end]
assert "vecptr == NULL || bufptr == NULL || swap_needed == NULL" in body
assert "vecptr->count < 0 || vecptr->count > RGMListSizeMax" in body
print("Mrm text-vector fixup guards pointers and counts")
