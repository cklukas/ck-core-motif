#!/usr/bin/env python3
"""Check DataField timer scrolling widens edge geometry before subtraction."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_CheckTimerScrolling(\n        Widget")
end = source.index("\nstatic void ", start)
region = source[start:end]

assert "int64_t margin_size" in region
assert "int64_t text_right" in region
assert "(int64_t)tf->core.width - margin_size" in region
assert "(tf->core.width - margin_size)" not in region
assert "df_ClampPointCoordinate" in region
assert "if (event == NULL)" in region
print("PASS DataField timer edge geometry is widened and clamped")
