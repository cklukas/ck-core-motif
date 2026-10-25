#!/usr/bin/env python3
"""Check that transfer merge allocation failure preserves conversion state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeConvertMerge(")
end = source.index("\nvoid\nXmeTransferAddDoneProc", start)
region = source[start:end]

assert "merged = XtMalloc((Cardinal)(offset + user_bytes));" in region
assert "if (merged == NULL)" in region
assert region.count("_XmProcessUnlock();") >= 3
assert "XtFree(cs->value);" in region
assert "cs->value = merged;" in region

print("PASS transfer merge allocation checks failure handling")
