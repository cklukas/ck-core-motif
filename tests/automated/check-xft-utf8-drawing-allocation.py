#!/usr/bin/env python3
"""Check that UTF-8 compound-string drawing handles conversion failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
needle = "ucs_str = _XmUtf8ToUcs2(draw_text, seg_len"
starts = [offset for offset in range(len(source)) if source.startswith(needle, offset)]
assert len(starts) == 2
regions = [source[offset:offset + 220] for offset in starts]

for region in regions:
    assert "if (ucs_str != NULL)" in region
    assert "XtFree((char *)ucs_str);" in region
    assert region.index("if (ucs_str != NULL)") < region.index("XDraw")
print("PASS UTF-8 compound-string drawing guards UCS-2 allocation")
