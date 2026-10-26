#!/usr/bin/env python3
"""Check that compound-string MB conversion handles result allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c").read_text(encoding="utf-8")
start = source.index("_XmGetMBStringFromXmString(XmString xmstr)")
end = source.index("/*\tFunction Name: _XiWoveWidget", start)
region = source[start:end]
assert region.count("text = XtMalloc((Cardinal)(length + 1));") == 2
assert region.count("if (text == NULL)") == 2
assert "XmStringFreeContext(context);" in region
print("PASS XmString MB conversion result allocations are guarded")
