#!/usr/bin/env python3
"""Check List horizontal origin arithmetic cannot wrap Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
assert "lw->list.XOrigin = bounded_position((long long)lw->list.MaxWidth -" in source
assert "lw->list.XOrigin = lw->list.MaxWidth - listwidth;" not in source
print("PASS List horizontal origin stays bounded")
