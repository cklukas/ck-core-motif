#!/usr/bin/env python3
"""Verify XmPaned narrow-margin paths cannot wrap or index before panes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text(
    encoding="utf-8"
)

assert "static Dimension SubtractMarginDimension(Dimension value, Dimension margin);" in source
helper = source[source.index("static Dimension\nSubtractMarginDimension"):]
assert "uint64_t extent = 2 * (uint64_t)margin;" in helper
assert "return (uint64_t)value > extent ? (Dimension)((uint64_t)value - extent) : 0;" in helper

adjust = source[source.index("AdjustPanedSize(XmPanedWidget") : source.index(
    "/************************************************************\n *\n *  Resolution", source.index("AdjustPanedSize(XmPanedWidget")
)]
assert adjust.count("AddMarginDimension(off_size") == 2
assert adjust.count("SubtractMarginDimension(*on_size_ret") == 2
assert adjust.count("SubtractMarginDimension(*off_size_ret") == 2

refigure = source[source.index("RefigureLocations(XmPanedWidget") : source.index(
    "/*\tFunction Name: CommitNewLocations", source.index("RefigureLocations(XmPanedWidget")
)]
assert "int fallback_index;" in refigure
assert "fallback_index = paneindex == NO_INDEX" in refigure
assert "? XmPaned_num_panes(pw) - 1" in refigure
assert ": (paneindex > 0 ? paneindex - 1 : 0);" in refigure
assert "NthPane(pw, fallback_index)" in refigure
assert "AddPanedDimension(PaneInfo(*NthPane(pw, fallback_index))->size," in refigure

reset = source[source.index("ResetSize(XmPanedWidget") : source.index(
    "/*\tFunction Name: Resize", source.index("ResetSize(XmPanedWidget")
)]
assert reset.count("SubtractMarginDimension(off_size") == 2

query = source[source.index("QueryGeometry(Widget w") : source.index(
    "/*\tFunction Name: PaneSetValues", source.index("QueryGeometry(Widget w")
)]
assert query.count("AddMarginDimension(") == 4

print("PASS XmPaned refigure margin arithmetic and fallback indexes are bounded")
