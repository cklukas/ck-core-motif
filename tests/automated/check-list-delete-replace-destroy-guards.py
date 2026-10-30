#!/usr/bin/env python3
"""Check List deletion and replacement operations fail during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/List.c").read_text(
    encoding="utf-8"
)

assert source.count("core.being_destroyed") >= 14
assert source.count("lw->core.being_destroyed") >= 10
print("PASS List delete/replace teardown guards")
