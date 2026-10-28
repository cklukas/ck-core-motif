#!/usr/bin/env python3
"""Check List combined horizontal draw origins cannot wrap Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
assert "x = bounded_position((long long)lw->list.BaseX + lw->list.XOrigin);" in source
assert "x = bounded_position((long long)lw->list.BaseX - lw->list.XOrigin);" in source
assert "x = lw->list.BaseX + lw->list.XOrigin;" not in source
assert "x = lw->list.BaseX - lw->list.XOrigin;" not in source
print("PASS List combined draw origin stays bounded")
