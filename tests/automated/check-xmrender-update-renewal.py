#!/usr/bin/env python3
"""Check shared rendition updates roll back when copy-on-write renewal fails."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("void\nXmRenditionUpdate(")
end = source.index("\n/*****************************************************************************/", start)
region = source[start:end]

assert "if (RenewRendition(rendition) == NULL)" in region
failure = region[region.index("if (RenewRendition(rendition) == NULL)"):]
failure = failure[:failure.index("can_free = FALSE")]
assert "_XmRendRefcountInc(rendition)" in failure
assert "goto update_done" in failure
assert "update_done:" in region
print("PASS shared rendition updates roll back failed renewal")
