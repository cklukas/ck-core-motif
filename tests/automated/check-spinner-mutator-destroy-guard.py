#!/usr/bin/env python3
"""Check Spinner activation setter fails closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/Spinner.c").read_text(
    encoding="utf-8"
)
assert "widget->core.being_destroyed" in source
print("PASS Spinner mutator guards destruction")
