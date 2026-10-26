#!/usr/bin/env python3
"""Check command convenience wrappers do not recursively lock text fields."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Command.c").read_text()
start = source.index("XmCommandSetValue(")
end = source.index("XmCommandAppendValue(", start)
function = source[start:end]

assert "_XmAppLock(app);" not in function
assert "XmTextFieldSetString(w->selection_box.text, str);" in function
assert "_XmAppUnlock(app);" not in function

print("PASS XmCommandSetValue avoids nested application locking")
