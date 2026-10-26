#!/usr/bin/env python3
"""Check that Paned separator-array allocation is bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Paned.c").read_text(
    encoding="utf-8"
)

assert "num_panes < 0" in source
assert "num_panes > SIZE_MAX / sizeof(Widget)" in source
assert "num_panes > UINT_MAX / sizeof(Widget)" in source
assert "sep_children == NULL && num_panes != 0" in source
print("Paned separator allocation is bounded")
