#!/usr/bin/env python3
"""Check ToggleButtonGadget pixmap redisplay clamps text geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nredisplayPixmap")
end = source.index("static void\nHandlePixmap", start)
body = source[start:end]

assert "static Position\ntogglebg_bounded_position(int64_t value)" in source
assert "LabG_TextRect_y(tb) = togglebg_bounded_position(MAX(h, 0));" in body
assert "LabG_TextRect_height(tb) = togglebg_bounded_dimension(onH);" in body
assert "LabG_TextRect_width(tb) = togglebg_bounded_dimension(onW);" in body
print("PASS ToggleButtonGadget pixmap redisplay clamps text geometry")
