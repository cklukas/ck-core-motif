#!/usr/bin/env python3
"""Check FileSB item filter construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
allocation = "itemString = XtMalloc( dirLen + maskLen + 1) ;"
start = source.index(allocation)
guard = source.index("if(    itemString != NULL    )", start)
end = source.index("XtFree( mask) ;", guard)
assert start < guard < end
assert "XmTextFieldSetString( FS_FilterText( fsb), itemString)" in source[guard:end]
print("PASS FileSB item filter allocation failure is safe")
