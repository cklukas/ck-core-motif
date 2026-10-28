#!/usr/bin/env python3
"""Ensure Mrm value fixup validates its byte-swap state pointer."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = text.index("Urm__CW_FixupValue (long")
body = text[start:text.index("\n}\n", start)]
assert "if (swap_needed == NULL)" in body
assert "if ( *swap_needed )" in body
print("Mrm value fixup guards the swap-state pointer")
