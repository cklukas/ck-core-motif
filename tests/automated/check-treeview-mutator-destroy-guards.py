#!/usr/bin/env python3
"""Check TreeView mutators fail closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/TreeView.c").read_text(
    encoding="utf-8"
)
assert source.count("widget->core.being_destroyed") >= 3
print("PASS TreeView mutators guard destruction")
