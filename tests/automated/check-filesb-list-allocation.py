#!/usr/bin/env python3
"""Check FileSB file-list conversion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "XmStringFileList = (XmString *) XtMalloc(numFiles * sizeof( XmString)) ;"
guard = "if(    XmStringFileList == NULL    )"
assert allocation in source
assert source.index(guard) > source.index(allocation)
assert "goto file_search_done ;" in source
assert "file_search_done:" in source
assert source.index("goto file_search_done ;") < source.index("file_search_done:")
print("PASS FileSB file-list allocation failure is cleaned up")
