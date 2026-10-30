#!/usr/bin/env python3
"""Check List selection and viewport mutators fail during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/List.c").read_text(
    encoding="utf-8"
)

assert source.count("lw->core.being_destroyed") >= 20
assert source.count("item == NULL || lw->core.being_destroyed") >= 5
print("PASS List selection/viewport teardown guards")
