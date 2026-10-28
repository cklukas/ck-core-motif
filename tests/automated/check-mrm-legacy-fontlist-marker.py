#!/usr/bin/env python3
"""Ensure legacy Mrm font-list fixup validates its marker."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_FixupValue (long")
body = text[start:text.index("\n}\n\n\n", start)]
old_pos = body.index("oldlist = (OldRGMFontListPtr)bufptr")
marker_pos = body.index("oldlist->validation != URMFontListValid", old_pos)
loop_pos = body.index("for ( fixndx=0 ; fixndx<oldlist->count", marker_pos)
assert old_pos < marker_pos < loop_pos
print("Mrm legacy font-list fixup validates its marker")
