#!/usr/bin/env python3
"""Check that default charset allocation failure uses a safe fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(encoding="utf-8")
start = source.index("_XmStringGetCurrentCharset")
end = source.index(" /* This function compares", start)
region = source[start:end]
assert "locale.tag = (char *) XtMalloc(len + 1);" in region
assert "if (locale.tag == NULL)" in region
assert "ret_val = XmFALLBACK_CHARSET;" in region
assert "_XmProcessUnlock();" in region
print("PASS XmString default charset allocation is guarded")
