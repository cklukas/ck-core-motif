#!/usr/bin/env python3
"""Check wide text-field setting avoids recursive locking and invalid free."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("XmTextFieldSetStringWcs(")
end = source.index("static void \nTextFieldReplace", start)
function = source[start:end]

assert "if (wc_value == NULL)" in function
assert "tmp[0] = '\\0';" in function
assert "_XmAppUnlock(app);\n  XmTextFieldSetString(w, tmp);" in function
assert "tmp = \"\"" not in function
assert "size_t result;" in function

print("PASS wide text-field setting avoids nested locking and invalid free")
