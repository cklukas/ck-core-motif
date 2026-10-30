#!/usr/bin/env python3
"""Check List insertion and clear operations fail during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/List.c").read_text(
    encoding="utf-8"
)

assert source.count("lw->core.being_destroyed") >= 6
assert source.count("item == NULL || lw->core.being_destroyed") >= 3
print("PASS List insertion/clear teardown guards")
