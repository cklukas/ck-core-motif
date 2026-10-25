#!/usr/bin/env python3
"""Check FileSB item filter construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "itemString = XtMalloc((Cardinal)(dirLen + maskLen + 1)) ;"
start = source.index(allocation)
guard = source.index("if(    itemString != NULL    )", start)
end = source.index("XtFree( mask) ;", guard)
assert start < guard < end
assert "size_t          dirLen" in source
assert "size_t          maskLen" in source
assert "maskLen == SIZE_MAX" in source[start - 300:guard]
assert "dirLen > SIZE_MAX - maskLen - 1" in source[start - 300:guard]
assert "dirLen + maskLen + 1 > UINT_MAX" in source[start - 300:guard]
assert "XmTextFieldSetString( FS_FilterText( fsb), itemString)" in source[guard:end]
print("PASS FileSB item filter allocation failure is safe")
