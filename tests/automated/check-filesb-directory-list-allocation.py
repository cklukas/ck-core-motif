#!/usr/bin/env python3
"""Check FileSB directory-list conversion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "XmStringDirList = (XmString *) XtMalloc( numDirs * sizeof( XmString)) ;"
guard = "if(    XmStringDirList == NULL    )"
assert allocation in source
assert source.index(guard) > source.index(allocation)
assert "goto dir_search_done ;" in source
assert "dir_search_done:" in source
assert source.index("goto dir_search_done ;") < source.index("dir_search_done:")
print("PASS FileSB directory-list allocation failure is cleaned up")
