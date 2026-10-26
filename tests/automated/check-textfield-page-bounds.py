#!/usr/bin/env python3
"""Ensure TextField paging uses a checked visible-width calculation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
for name in ("PageRight(Widget w,", "PageLeft(Widget w,"):
    start = source.rindex(name)
    end = source.index("\n/* ARGSUSED */", start)
    region = source[start:end]
    assert "uint64_t margin_width" in region
    assert "margin_width >= (uint64_t)tf->core.width / 2" in region
    assert "viewport_width = (int)((uint64_t)tf->core.width - 2 * margin_width)" in region

print("TextField paging uses checked visible-width arithmetic")
