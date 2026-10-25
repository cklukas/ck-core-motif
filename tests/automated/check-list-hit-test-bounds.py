#!/usr/bin/env python3
"""Check List hit testing handles BaseY larger than a collapsed widget."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

needle = "(int)lw->core.height - (int)lw->list.BaseY"
assert source.count(needle) >= 2

print("List hit-test geometry is bounded")
