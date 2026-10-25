#!/usr/bin/env python3
"""Check XmString table conversion avoids recursive locking and cleans failure."""

from pathlib import Path


source = Path("lib/Xm/XmStringFunc.c").read_text()
start = source.index("XmStringToXmStringTable(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "if (*table == NULL) {\n\t_XmStringContextFree(&stack_context);" in function
assert "XmeStringGetComponent" in function

print("XmString table conversion avoids recursive locking and cleans failure")
