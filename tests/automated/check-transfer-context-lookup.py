#!/usr/bin/env python3
"""Check transfer context lookup handles allocation and insertion races."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static ConvertContext\nLookupContextBlock(")
end = source.index("static void\nClearContextBlock", start)
body = source[start:end]

assert "ConvertHashTable == (XmHashTable) NULL ? NULL :" in body
assert "if (ConvertHashTable == (XmHashTable) NULL)" in body
assert "_XmGetHashEntry(ConvertHashTable," in body
assert "if (!_XmAddHashEntry(ConvertHashTable" in body
assert "XtFree((char *)new_cc);" in body
assert body.count("_XmProcessLock();") == 2
assert body.count("_XmProcessUnlock();") >= 4

print("transfer context lookup handles allocation and insertion races")
