#!/usr/bin/env python3
"""Check that the shared XmString triple reader guards null contexts."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmeStringGetComponent(_XmStringContext context,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (context == NULL || ! (length  && value))" in function
assert "return XmSTRING_COMPONENT_END;" in function

print("XmString triple reader guards null contexts")
