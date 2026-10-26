#!/usr/bin/env python3
"""Ensure TextField baseline geometry is widened before returning int."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("XmTextFieldGetBaseline(")
end = source.index("\n/*\n * Function:\n *    XmCreateTextField", start)
region = source[start:end]

assert "int64_t baseline;" in region
assert "(int64_t)tf->text.margin_top" in region
assert "if (baseline > INT_MAX)" in region
assert "baseline = INT_MAX;" in region
assert "else if (baseline < INT_MIN)" in region
assert "ret_val = (int)baseline;" in region

print("TextField baseline geometry is widened and clamped")
