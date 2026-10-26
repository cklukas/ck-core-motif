#!/usr/bin/env python3
"""Check FileSB directory-list conversion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "XmStringFileList = (XmString *) XtMalloc("
guard = "if(    XmStringFileList == NULL    )"
assert allocation in source
assert "numFiles > SIZE_MAX / sizeof(XmString)" in source
assert source.index(guard) > source.index(allocation)
assert "goto file_search_cleanup;" in source
assert "file_search_cleanup:" in source
assert source.index("goto file_search_cleanup;") < source.index("file_search_cleanup:")
print("PASS FileSB directory-list allocation failure is cleaned up")
