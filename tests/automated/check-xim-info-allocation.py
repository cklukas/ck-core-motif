#!/usr/bin/env python3
"""Check that XIM info and preedit-buffer allocations are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("create_xic_info(Widget")
end = source.index("#define IsCallback", start)
region = source[start:end]
assert "xic_info = XtNew(XmImXICRec);" in region
assert "if (xic_info == NULL)" in region
assert "xic_info->preedit_buffer = XtNew(PreeditBufferRec);" in region
assert "if (xic_info->preedit_buffer == NULL)" in region
assert "XtFree((char *)xic_info);" in region
print("PASS XIM info allocations are transactional")
