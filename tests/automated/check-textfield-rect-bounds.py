#!/usr/bin/env python3
"""Ensure TextField clip-rectangle margins use widened arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("GetRect(XmTextFieldWidget tf,")
end = source.index("\nstatic void \nSetFullGC", start)
region = source[start:end]

assert "uint64_t margin_width" in region
assert "uint64_t margin_top" in region
assert "uint64_t margin_bottom" in region
assert "2 * margin_width" in region
assert "margin_top + margin_bottom" in region
assert "SHRT_MAX" in region
assert "Dimension margin_width" not in region

print("TextField clip-rectangle margins use widened bounded arithmetic")
