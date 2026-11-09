#!/usr/bin/env python3
"""Check Stepper only narrows already-bounded layout values."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text()
for declaration in ("Dimension control_height", "Dimension button_width",
                    "Dimension label_width"):
    assert declaration in source
assert "(Position)(rtl?width-bw:0)" in source
assert "(Position)(bw+gap)" in source
print("PASS Stepper layout conversions are explicit and bounded")
