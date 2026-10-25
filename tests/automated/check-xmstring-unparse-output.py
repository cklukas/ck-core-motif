#!/usr/bin/env python3
"""Check XmStringUnparse validates output text types."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmStringUnparse(XmString")
end = source.index("\n  _XmProcessLock();", start)
region = source[start:end]

assert "output_type != XmCHARSET_TEXT" in region
assert "output_type != XmMULTIBYTE_TEXT" in region
assert "output_type != XmWIDECHAR_TEXT" in region
assert "output_type != XmNO_TEXT" in region
assert "return NULL;" in region

print("XmStringUnparse validates output text types")
