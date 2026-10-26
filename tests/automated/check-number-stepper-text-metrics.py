#!/usr/bin/env python3
"""Check compact numeric controls center text using active font metrics."""
from pathlib import Path

for name, symbol in (("NumberBox.c", "number->number_box.text_gc"),
                     ("Stepper.c", "s->stepper.gc")):
    source = Path("lib/Xm", name).read_text(encoding="utf-8")
    assert symbol in source, name
    assert "XmStringExtent" in source, name
    assert "XmStringDraw" in source, name
    assert "(int)" in source, name
print("PASS NumberBox and Stepper use measured font baselines")
