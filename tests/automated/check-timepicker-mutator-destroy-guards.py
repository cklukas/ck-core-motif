#!/usr/bin/env python3
"""Check TimePicker value/range setters fail closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/TimePicker.c").read_text(
    encoding="utf-8"
)
assert source.count("widget->core.being_destroyed") >= 2
print("PASS TimePicker mutators guard destruction")
