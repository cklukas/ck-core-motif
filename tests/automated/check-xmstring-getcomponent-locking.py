#!/usr/bin/env python3
"""Check XmStringGetNextComponent does not recursively lock its helper."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringGet.c").read_text()
start = source.index("XmStringGetNextComponent(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "XmeStringGetComponent((_XmStringContext) context" in function

print("PASS XmStringGetNextComponent avoids recursive process locking")
