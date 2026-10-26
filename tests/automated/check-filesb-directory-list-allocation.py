#!/usr/bin/env python3
"""Check FileSB directory-list conversion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "XmStringDirList = (XmString *) XtMalloc("
guard = "if(    XmStringDirList == NULL    )"
assert allocation in source
assert "XmString *      XmStringDirList = NULL" in source
assert "numDirs > SIZE_MAX / sizeof(XmString)" in source
assert source.index(guard) > source.index(allocation)
assert "goto dir_string_cleanup;" in source
assert "dir_string_cleanup:" in source
assert source.index("goto dir_string_cleanup;") < source.index("dir_string_cleanup:")
print("PASS FileSB directory-list allocation failure is cleaned up")
