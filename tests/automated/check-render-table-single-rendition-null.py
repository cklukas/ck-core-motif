#!/usr/bin/env python3
"""Check XmRenderTableGetRendition rejects a null table safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableGetRendition(")
end = source.index("/* Returns array of copies of matching renditions. */", start)
region = source[start:end]

assert "if (table == NULL)" in region
assert "return(NULL);" in region
assert region.index("if (table == NULL)") < region.index("if (_XmRTDisplay(table))")
assert "if (_XmRTDisplay(table))" in region
assert "_XmProcessLock();" in region
assert "_XmProcessUnlock();" in region
print("PASS render-table single-rendition lookup handles null tables")
