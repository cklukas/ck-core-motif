#!/usr/bin/env python3
"""Check Container placement and hiding avoid wrapped Position casts."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
place_start = source.index("PlaceCwid(\n    Widget")
place = source[place_start:source.index("SnapCwid(", place_start)]
hide_start = source.index("HideCwid(\n    Widget")
hide = source[hide_start:source.index("SetValues(", hide_start)]

for needle in (
    "int64_t right_limit",
    "(int64_t)cwid->core.width +",
    "AdjustPositionSaturated(0, right_limit)",
    "AdjustPositionSaturated(0,\n\t    (int64_t)cw->container.margin_w)",
):
    assert needle in place, f"missing bounded Container placement arithmetic: {needle}"
assert "(Position)cw->container.margin_w" not in place
assert "AdjustPositionSaturated(0,\n\t     -(int64_t)((Widget)cwid)->core.width)" in hide
assert "(Position)(0 -" not in hide
print("PASS Container placement and hiding use bounded Position arithmetic")
