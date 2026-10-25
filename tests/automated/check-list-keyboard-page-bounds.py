#!/usr/bin/env python3
"""Check List horizontal keyboard paging uses signed viewport width."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

needle = "(int)lw->core.width - CHAR_WIDTH_GUESS"
assert source.count(needle) >= 4

print("List keyboard page arithmetic is bounded")
