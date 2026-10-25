#!/usr/bin/env python3
"""Check FileSB directory-list cleanup accepts failed allocation paths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
declaration = source.index("XmString *      XmStringDirList")
start = source.rfind("static void", 0, declaration)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "XmString *      XmStringDirList = NULL ;" in function
assert "goto dir_string_cleanup;" in function
assert "XtFree((char *)XmStringDirList);" in function
print("PASS FileSB directory-list cleanup initialization")
