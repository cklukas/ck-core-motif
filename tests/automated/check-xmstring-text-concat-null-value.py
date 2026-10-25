#!/usr/bin/env python3
"""Check that text concatenation rejects non-empty null components."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringGetTextConcat(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (len != 0 && val == NULL)" in function
assert "_XmStringContextFree(&stack_context);" in function
assert "XtFree(OutStr);" in function
assert "return NULL;" in function

print("XmString text concatenation rejects null component values")
