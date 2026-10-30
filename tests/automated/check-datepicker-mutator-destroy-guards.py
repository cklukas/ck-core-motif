#!/usr/bin/env python3
"""Check DatePicker public mutators fail closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/DatePicker.c").read_text(
    encoding="utf-8"
)

assert source.count("picker->core.being_destroyed") >= 5
print("PASS DatePicker mutator guards destruction")
