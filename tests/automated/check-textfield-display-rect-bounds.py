#!/usr/bin/env python3
"""Ensure TextField display-rectangle geometry is widened and guarded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nTextFieldGetDisplayRect(Widget w,")
end = source.index("TextFieldMarginsProc(Widget w,", start)
region = source[start:end]
assert "if (!display_rect) return False" in region
assert "GetRect((XmTextFieldWidget)w, display_rect)" in region
start = source.index("static void \nGetRect(XmTextFieldWidget tf,")
end = source.index("static void \nSetFullGC", start)
region = source[start:end]
assert "uint64_t margin_width" in region
assert "uint64_t right_margin" in region
assert "margin_width + right_margin <= (uint64_t)tf->core.width" in region
assert "margin_top + margin_bottom" in region
assert "SHRT_MAX" in region
print("TextField display rectangles share widened, asymmetric text clipping geometry")
