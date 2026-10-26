#!/usr/bin/env python3
"""Check Stepper public range/value APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text(encoding="utf-8")
assert source.count("widget == NULL || !XmIsStepper(widget)") >= 3
assert "widget != NULL && XmIsStepper(widget)" in source
print("PASS Stepper range/value APIs validate null widgets")
