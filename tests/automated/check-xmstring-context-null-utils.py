#!/usr/bin/env python3
"""Check XmString context utility null guards."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

for signature in (
    "_XmStringContextReInit(_XmStringContext context,",
    "_XmStringContextCopy(_XmStringContext target,",
    "_XmStringContextFree(_XmStringContext context)",
):
    start = source.index(signature)
    end = source.index("\n}\n", start)
    function = source[start:end]
    assert "return;" in function

assert "assert(context != NULL);" not in source
assert "assert(source && target && (source != target));" not in source

print("XmString context utilities guard null pointers")
