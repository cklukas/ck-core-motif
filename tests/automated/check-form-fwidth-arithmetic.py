#!/usr/bin/env python3
"""Check Form fallback width/height arithmetic remains signed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Form.c").read_text()

assert source.count("temp1 = (int)*fwidth - GetFormOffset") >= 4
assert source.count("temp1 = (int)*fheight - GetFormOffset") >= 4
assert "((int)*fwidth - (int)size" in source
assert "((int)*fheight - (int)size" in source
assert "static Dimension\nFormAddDimension(Dimension value, double addition)" in source
assert "if (addition >= (double)USHRT_MAX - value)" in source
assert "*fwidth +=" not in source
assert "*fheight +=" not in source

print("Form measured-size arithmetic is bounded")
