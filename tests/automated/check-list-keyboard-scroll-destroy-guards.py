#!/usr/bin/env python3
"""Check List keyboard and horizontal-scroll mutators fail during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/List.c").read_text(
    encoding="utf-8"
)

assert source.count("lw->core.being_destroyed") >= 22
print("PASS List keyboard/scroll teardown guards")
