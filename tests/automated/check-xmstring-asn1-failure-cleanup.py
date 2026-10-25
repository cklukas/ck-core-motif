#!/usr/bin/env python3
"""Check ASN.1 XmString conversion cleans partial state on allocation failure."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

assert "goto conversion_failed;" in source
assert "conversion_failed:\n  XtFree((char *)_XmUnoptSegRendBegins(&seg));" in source
assert "new_entries = (_XmStringEntry *)XtMalloc(sizeof(_XmStringEntry));" in source
assert "if (line == NULL) {\n\t      XtFree((char *)new_entries);" in source
assert "_XmUnoptSegRendBeginCount(&seg) = rend_cnt;" in source
assert "_XmUnoptSegRendEndCount(&seg) = rend_cnt;" in source

print("XmString ASN.1 conversion cleans allocation failures")
