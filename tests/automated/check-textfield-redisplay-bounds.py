#!/usr/bin/env python3
"""Ensure TextField redisplay geometry checks widened margins."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("RedisplayText(XmTextFieldWidget tf,")
end = source.index("\nstatic Boolean \nAdjustText", start)
region = source[start:end]

assert "uint64_t margin_width" in region
assert "uint64_t margin_top" in region
assert "uint64_t margin_bottom" in region
assert "margin_width > (uint64_t)tf->core.width / 2" in region
assert "margin_top + margin_bottom >= (uint64_t)tf->core.height" in region
assert "int64_t baseline_y" in region
assert "if (baseline_y > INT_MAX)" in region
assert "Dimension margin_width" not in region

print("TextField redisplay geometry uses widened bounded arithmetic")
