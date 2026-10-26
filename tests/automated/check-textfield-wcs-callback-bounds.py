#!/usr/bin/env python3
"""Ensure TextField WCS callback buffers are bounded and checked."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (TextF_ModifyVerifyCallbackWcs(tf) && vcb.doit)")
end = text.index("\n    wcs_do_free = True;", start)
block = text[start:end]

assert block.count("SIZE_MAX / sizeof(wchar_t) - 1") == 2
assert block.count("UINT_MAX / sizeof(wchar_t)") == 2
assert block.count("wcs_newblock.wcsptr == NULL") == 2
assert "XtFree(newblock.ptr);" in block
assert "return(False);" in block

print("TextField WCS callback bounds: ok")
