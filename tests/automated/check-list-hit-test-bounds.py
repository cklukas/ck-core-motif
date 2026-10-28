#!/usr/bin/env python3
"""Check List hit-test height arithmetic does not wrap Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
assert "(long long)y >= (long long)lw->core.height - lw->list.BaseY" in source
assert source.count("(Position)((int)lw->core.height - (int)lw->list.BaseY)") == 0
print("PASS List hit-test height arithmetic stays signed")
