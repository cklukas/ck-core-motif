#!/usr/bin/env python3
"""Check Gadget default geometry expands without wrapping Dimension."""

from pathlib import Path


source = Path("lib/Xm/Gadget.c").read_text(encoding="utf-8")

helper_start = source.index("GadgetDefaultDimension(Dimension size")
helper_end = source.index("/************************************************************************", helper_start)
helper = source[helper_start:helper_end]
assert "uint64_t total" in helper
assert "2 * (uint64_t)highlight" in helper
assert "2 * (uint64_t)shadow" in helper
assert "total > (uint64_t)USHRT_MAX" in helper
assert "(Dimension)USHRT_MAX" in helper

initialize_start = source.index("Initialize(\n")
initialize_end = source.index("static Boolean \nSetValues(", initialize_start)
initialize = source[initialize_start:initialize_end]
assert initialize.count("GadgetDefaultDimension(gw->rectangle") == 2
assert "gw->rectangle.width +=" not in initialize
assert "gw->rectangle.height +=" not in initialize

print("Gadget default dimensions saturate instead of wrapping")
