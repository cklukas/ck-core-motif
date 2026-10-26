#!/usr/bin/env python3
"""Check that UTF-8 to UCS-2 conversion handles allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Xm.c"
text = source.read_text()
start = text.index("_XmUtf8ToUcs2(")
end = text.index("\n\n", text.index("return buf2b;", start))
block = text[start:end]

if "buf2b = (XChar2b *)XtMalloc" not in block:
    raise SystemExit("UTF-8 conversion allocation is missing")
if "seg_len > (size_t)UINT_MAX / sizeof(XChar2b)" not in block:
    raise SystemExit("UTF-8 conversion lacks Xt allocation-width guard")
if "if (buf2b == NULL)" not in block:
    raise SystemExit("UTF-8 conversion allocation has no null guard")
if "*ret_str_len = 0" not in block:
    raise SystemExit("UTF-8 conversion failure does not reset output length")

print("UTF-8 to UCS-2 conversion handles allocation failure")
