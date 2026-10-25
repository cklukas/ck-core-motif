#!/usr/bin/env python3
"""Check tab-list proposal delegates locking to its helpers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableProposeTablist(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmProcessLock();" not in function
assert "_XmProcessUnlock();" not in function
assert "XmTabCreate" in function
assert "XmTabListInsertTabs" in function
assert "_XmStringGetNextTabWidth" in function

print("PASS XmString tab-list proposal avoids recursive process locking")
