#!/usr/bin/env python3
"""Check DataField position geometry widens edge arithmetic and clamps output."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_GetXYFromPos(\n        XmDataFieldWidget tf")
start = source.index("{", start)
end = source.index("\nstatic Boolean ", start)
region = source[start:end]

assert "int64_t x_coordinate, y_coordinate" in region
assert "(int64_t)tf->core.width -" in region
assert "df_ClampPointCoordinate(x_coordinate)" in region
assert "df_ClampPointCoordinate(y_coordinate)" in region
assert "*x = tf->core.width - (XmTextF_margin_width(tf)" not in region
print("PASS DataField position geometry is widened and clamped")
