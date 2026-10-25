#!/usr/bin/env python3
"""Check render-table export rejects malformed entry and font-name state."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtToProp(")
end = source.index("\n\ntypedef enum", start)
region = source[start:end]

assert "*prop_return = NULL" in region
assert "rendition == NULL || _XmRendTag(rendition) == NULL" in region
assert "_XmRendFontName(rendition) == NULL" in region
print("PASS XmRenderTableCvtToProp rejects malformed rendition state")
