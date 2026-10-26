#!/usr/bin/env python3
"""Ensure Text autoscroll bottom bounds use widened metric arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean \nCheckTimerScrolling(")
end = source.index("\n/* ARGSUSED */\nstatic void\nStartExtendSelection", start)
region = source[start:end]

assert "int64_t text_bottom;" in region
assert "(int64_t)o_data->lineheight * (int64_t)o_data->number_lines" in region
assert "(int64_t)event->xmotion.y < text_bottom" in region
assert "(int64_t)text_bottom" in region
assert "o_data->lineheight * o_data->number_lines)))" not in region

print("Text autoscroll: bottom bounds use widened metric arithmetic")
