#!/usr/bin/env python3
"""Ensure RTL autoscroll edge arithmetic is widened before comparison."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean \nCheckTimerScrolling(")
end = source.index("\n/* ARGSUSED */\nstatic void\nStartExtendSelection", start)
region = source[start:end]

assert "int64_t rtl_left_edge;" in region
assert "(int64_t)o_data->linewidth * ((int64_t)o_data->number_lines + 1)" in region
assert "(int64_t)rtl_left_edge" in region
assert "ClampTextPointCoordinate(rtl_left_edge)" in region
assert "o_data->linewidth * (o_data->number_lines + 1))))" not in region

print("Text RTL autoscroll: left edge arithmetic is widened")
