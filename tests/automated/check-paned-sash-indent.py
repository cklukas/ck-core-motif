#!/usr/bin/env python3
"""Check Paned negative sash indents avoid unsigned coordinate wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "int\toffset, sash_size, sash_pos;" in source
assert "(int) pw->core.width +" in source
assert "(int) pw->core.height +" in source
assert "changes.x = (Position) sash_pos;" in source
assert "changes.y = (Position) sash_pos;" in source

print("Paned negative sash indents are signed")
