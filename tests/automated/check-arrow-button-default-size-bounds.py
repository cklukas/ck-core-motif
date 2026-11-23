#!/usr/bin/env python3
"""Check ArrowButton default geometry cannot wrap Dimension values."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"

for name, part in (("ArrowB.c", "core"), ("ArrowBG.c", "rectangle")):
    source = (root / name).read_text()
    start = source.index("\nInitialize(")
    initializer = source[start:source.index("\n}", start)]
    assert "Dimension extent = _XmAppearanceArrowExtent(" in initializer
    for axis in ("width", "height"):
        assert f"if (request->{part}.{axis} == 0) new_w->{part}.{axis} = extent;" in initializer

drawing = (root / "AppearanceDraw.c").read_text()
start = drawing.index("\n_XmAppearanceArrowExtent(")
helper = drawing[start:drawing.index("\n}", start)]
assert "unsigned long extent = _XmDpiDefaultDimension(widget, 15) + margins;" in helper
assert "return (Dimension)MIN(extent,USHRT_MAX);" in helper
print("PASS ArrowButton and ArrowButtonGadget default dimensions saturate")
