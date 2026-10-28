#!/usr/bin/env python3
"""Ensure current Mrm font-list fixup validates post-swap state."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_FixupValue (long")
body = text[start:text.index("\n}\n\n\n", start)]
swap_pos = body.index("swapbytes( fontlist->count )")
validation_pos = body.index("fontlist->validation != URMFontListValid", swap_pos)
loop_pos = body.index("for ( fixndx=0 ; fixndx<fontlist->count", validation_pos)
assert swap_pos < validation_pos < loop_pos
assert "fontlist->count < 0" in body[validation_pos:loop_pos]
print("Mrm font-list fixup validates marker and count after swapping")
