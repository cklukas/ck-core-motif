#!/usr/bin/env python3
"""Ensure TextField AdjustText widens margin and fill geometry."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("AdjustText(XmTextFieldWidget tf,")
end = source.index("\n/*\n * AdjustSize", start)
region = source[start:end]

assert "uint64_t margin_width" in region
assert "uint64_t thickness" in region
assert "margin_width >= (uint64_t)tf->core.width" in region
assert "(uint64_t)tf->core.height <= thickness" in region
assert "Dimension margin_width" not in region

print("TextField AdjustText uses widened bounded margin geometry")
