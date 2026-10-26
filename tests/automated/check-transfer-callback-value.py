#!/usr/bin/env python3
"""Check clipboard callback conversion values are initialized per request."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text(encoding="utf-8")

assert "XtPointer value = NULL;" in source
assert "if (*reason != XmCR_CLIPBOARD_DATA_DELETE) {\n    value = NULL;" in source
assert "/* If this was the last item" in source
assert "*/\n    value = NULL;\n    _XmConvertHandlerSetLocal();" in source

print("clipboard callback-value checks passed")
