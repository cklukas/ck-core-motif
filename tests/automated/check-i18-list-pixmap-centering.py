#!/usr/bin/env python3
"""Check I18List pixmap centering does not underflow unsigned heights."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "pix_y_offset = bounded_short(" in source
assert "pix_y_offset = (height - XmI18List_row_data(ilist)[row].pix_height)/2;" not in source
print("PASS I18List pixmap centering stays signed")
