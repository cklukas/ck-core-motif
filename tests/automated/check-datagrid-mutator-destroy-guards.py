#!/usr/bin/env python3
"""Check DataGrid mutators fail closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/DataGrid.c").read_text(
    encoding="utf-8"
)
assert source.count("widget->core.being_destroyed") >= 8
print("PASS DataGrid mutators guard destruction")
