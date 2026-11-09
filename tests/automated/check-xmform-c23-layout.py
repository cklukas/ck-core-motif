#!/usr/bin/env python3
"""Verify xmform's typed helpers and empty-word layout guard."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "unsupported"
    / "xmform"
    / "xmform.c"
).read_text(encoding="utf-8")

assert "int main(int argc, char **argv)" in source
assert "static void FillItPlease(Widget, char);" in source
assert "static void NearlyEvenSpread(Widget *, Cardinal, Dimension, Dimension);" in source
assert "static Widget AddStem(Widget, Dimension, Dimension, Dimension, Dimension, int);" in source
assert "if (word[0] == '\\0') word = \" \";" in source
assert "if (num_child == 0) return;" in source
assert "AddStem(form, 0, 75, 35, 25, 0)" in source
assert "AddStem(form, 0, 63, 25, 38, 0)" in source

print("xmform uses typed C23 helpers and protects empty-word layout")
