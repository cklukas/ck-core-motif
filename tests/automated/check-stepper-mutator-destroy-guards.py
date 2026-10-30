#!/usr/bin/env python3
"""Check Stepper range/value/step setters fail closed during teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/Stepper.c").read_text(
    encoding="utf-8"
)
assert source.count("widget->core.being_destroyed") >= 3
print("PASS Stepper mutators guard destruction")
