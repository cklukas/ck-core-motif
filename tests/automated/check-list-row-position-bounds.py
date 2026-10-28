#!/usr/bin/env python3
"""Check List row-height accumulation cannot wrap Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
assert "(long long)(lines) * ((long long)(lw)->list.MaxItemHeight" in source
assert "y = bounded_position(LINEHEIGHTS" in source
assert source.count("y = bounded_position(LINEHEIGHTS") >= 2
assert "y = LINEHEIGHTS(lw, pos - lw->list.top_position) + lw->list.BaseY;" not in source
print("PASS List row-position accumulation stays wide and bounded")
