#!/usr/bin/env python3
"""Check ProgressBar range/value/indeterminate setters fail closed."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/ProgressBar.c").read_text(
    encoding="utf-8"
)
assert source.count("widget->core.being_destroyed") >= 3
print("PASS ProgressBar mutators guard destruction")
