#!/usr/bin/env python3
"""Ensure secondary extension does not use absent-selection output values."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nDoSecondaryExtend(Widget w,")
end = source.index("\n\n/************************************************************************", start)
region = source[start:end]

assert "if (!_XmTextGetSel2(tw, &left, &right)) {" in region
assert "_XmTextEnableRedisplay(tw);" in region
assert "return;" in region

get_pos = region.index("_XmTextGetSel2(tw, &left, &right)")
scan_pos = region.index("(*tw->text.source->Scan)")
assert get_pos < scan_pos

print("secondary selection extension: absent selection exits before use")
