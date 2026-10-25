#!/usr/bin/env python3
"""Check XmRenderTableGetRenditions keeps its documented positional array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableGetRenditions(")
end = source.index("/* Wrapper for calling XtWarning functions. */", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(*rends)" in region
assert "UINT_MAX / sizeof(*rends)" in region
assert "XtMalloc((size_t)tag_count * sizeof(*rends))" in region
assert "memset(rends, 0, (size_t)tag_count * sizeof(*rends));" in region
assert "rends[i] = CopyRendition(rend);" in region
assert "XtRealloc" not in region
assert "if (count == 0 || copy_failed)" in region
assert "if (_XmRTDisplay(table))" in region
assert "_XmProcessLock();" in region
assert "_XmProcessUnlock();" in region
assert "FreeRendition(rends[i]);" in region
assert "FreeHandle(rends[i]);" in region
assert "XmRenditionFree(rends[i]);" not in region
print("PASS render-table rendition result preserves tag positions")
