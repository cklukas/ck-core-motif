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

append_start = source.index("XmCommandAppendValue(")
append_end = source.index("XmCommandError(", append_start)
append = source[append_start:append_end]
assert "_XmAppLock(app);" not in append
assert "XmTextFieldGetLastPosition" in append
assert "XmTextFieldReplace" in append
assert "XmTextFieldSetInsertionPosition" in append
assert "_XmAppUnlock(app);" not in append
print("PASS XmCommandAppendValue avoids nested application locking")

error_start = source.index("XmCommandError(")
error_end = source.index("XmCreateCommandDialog(", error_start)
error = source[error_start:error_end]
assert "_XmAppLock(app);" not in error
assert "XmListDeletePos" in error
assert "XmListAddItemUnselected" in error
assert "XmListSetBottomPos" in error
assert "_XmAppUnlock(app);" not in error
print("PASS XmCommandError avoids nested application locking")
