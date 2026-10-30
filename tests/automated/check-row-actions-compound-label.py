#!/usr/bin/env python3
"""Check RowActions builds visible labels without flattening XmStrings."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("static XmString button_label")
end = source.index("static void", start)
body = source[start:end]
assert "XmStringConcat" in body
assert "XmStringUnparse" not in body
assert "XmStringCopy(shortcut)" in body
assert "button_text" not in source
assert "char *text; XmString label;" not in source

print("RowActions visible labels preserve compound-string segments")
